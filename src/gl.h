#ifndef XEMMAIX__GL__GL_H
#define XEMMAIX__GL__GL_H

#include <xemmai/convert.h>
#ifdef _WIN32
#include <GL/glew.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

namespace xemmaix::gl
{

using namespace xemmai;
using namespace xemmai::portable;

class t_library;
template<typename> class t_array_of;
class t_error;
class t_buffer;
class t_framebuffer;
class t_renderbuffer;
class t_texture;
class t_program;
class t_shader;
class t_uniform_location;

class t_session
{
	friend class t_error;
	friend class t_buffer;
	friend class t_framebuffer;
	friend class t_renderbuffer;
	friend class t_texture;
	friend class t_program;
	friend class t_shader;

	static std::atomic_flag v_running;
	static XEMMAI__PORTABLE__THREAD t_session* v_instance;
#ifdef _WIN32
	static bool v_glew;
#endif

	t_library* v_library;
	std::map<GLuint, t_root> v_buffers;
	std::map<GLuint, t_root> v_framebuffers;
	std::map<GLuint, t_root> v_renderbuffers;
	std::map<GLuint, t_root> v_textures;
	std::map<GLuint, t_root> v_programs;
	std::map<GLuint, t_root> v_shaders;

public:
	static t_session* f_instance()
	{
		if (!v_instance) f_throw(L"must be inside main."sv);
#ifdef _WIN32
		if (!v_glew) {
			if (glewInit() != GLEW_OK) f_throw(L"glewInit failed."sv);
			if (GLEW_VERSION_2_0 == GL_FALSE) f_throw(L"GL 2.0 is required."sv);
			v_glew = true;
		}
#endif
		return v_instance;
	}

	t_session(t_library* a_library);
	~t_session();
	t_library* f_library() const
	{
		return v_library;
	}
};

class t_library : public xemmai::t_library
{
	t_slot_of<t_type> v_type_array_of_int16;
	t_slot_of<t_type> v_type_array_of_int32;
	t_slot_of<t_type> v_type_array_of_float32;
	t_slot_of<t_type> v_type_error;
	t_slot_of<t_type> v_type_buffer;
	t_slot_of<t_type> v_type_framebuffer;
	t_slot_of<t_type> v_type_renderbuffer;
	t_slot_of<t_type> v_type_texture;
	t_slot_of<t_type> v_type_program;
	t_slot_of<t_type> v_type_shader;
	t_slot_of<t_type> v_type_uniform_location;

public:
	using xemmai::t_library::t_library;
	virtual void f_scan(t_scan a_scan);
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();
	template<typename T>
	const T* f_library() const
	{
		return f_global();
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return f_global()->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_library*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_library<typename t::t_library>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_library* t_library::f_library<t_library>() const
{
	return this;
}

XEMMAI__LIBRARY__TYPE_AS(t_library, t_array_of<short>, array_of_int16)
XEMMAI__LIBRARY__TYPE_AS(t_library, t_array_of<int>, array_of_int32)
XEMMAI__LIBRARY__TYPE_AS(t_library, t_array_of<float>, array_of_float32)
XEMMAI__LIBRARY__TYPE(t_library, error)
XEMMAI__LIBRARY__TYPE(t_library, buffer)
XEMMAI__LIBRARY__TYPE(t_library, framebuffer)
XEMMAI__LIBRARY__TYPE(t_library, renderbuffer)
XEMMAI__LIBRARY__TYPE(t_library, texture)
XEMMAI__LIBRARY__TYPE(t_library, program)
XEMMAI__LIBRARY__TYPE(t_library, shader)
XEMMAI__LIBRARY__TYPE(t_library, uniform_location)

template<typename T>
struct t_holds : t_bears<T>
{
	template<typename T0>
	struct t_cast
	{
		template<typename T1>
		static T0& f_call(T1&& a_object)
		{
			xemmaix::gl::t_session::f_instance();
			auto& p = t_base::f_object(std::forward<T1>(a_object))->template f_as<T0>();
			if (p.v_entry == decltype(p.v_entry){}) f_throw(L"already destroyed."sv);
			return p;
		}
	};
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			return t_cast<typename t_fundamental<T0>::t_type>::f_call(std::forward<T1>(a_object));
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			return t_base::f_object(std::forward<T1>(a_object)) ? &t_cast<T0>::f_call(std::forward<T1>(a_object)) : nullptr;
		}
	};
	template<typename T0>
	struct t_is
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = t_base::f_object(std::forward<T1>(a_object));
			return reinterpret_cast<uintptr_t>(p) >= e_tag__OBJECT && p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
		}
	};
	template<typename T0>
	struct t_is<T0*>
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = t_base::f_object(std::forward<T1>(a_object));
			switch (reinterpret_cast<uintptr_t>(p)) {
			case e_tag__NULL:
				return true;
			case e_tag__BOOLEAN:
			case e_tag__INTEGER:
			case e_tag__FLOAT:
				return false;
			default:
				return p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
			}
		}
	};
	typedef xemmaix::gl::t_library t_library;
	typedef t_holds t_base;

	using t_bears<T>::t_bears;
	static void f_do_finalize(t_object* a_this)
	{
		auto& p = a_this->f_as<T>();
		assert(p.v_entry == decltype(p.v_entry){});
		p.~T();
	}
};

}

#endif

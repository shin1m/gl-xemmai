#ifndef XEMMAIX__GL__GL_H
#define XEMMAIX__GL__GL_H

#include <map>
#include <xemmai/convert.h>
#include <xemmai/tuple.h>
#include <xemmai/bytes.h>
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

class t_extension;
template<typename T>
class t_array_of;
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

	static std::mutex v_mutex;
	static bool v_running;
	static XEMMAI__PORTABLE__THREAD t_session* v_instance;
#ifdef _WIN32
	static bool v_glew;
#endif

	t_extension* v_extension;
	std::map<GLuint, t_scoped> v_buffers;
	std::map<GLuint, t_scoped> v_framebuffers;
	std::map<GLuint, t_scoped> v_renderbuffers;
	std::map<GLuint, t_scoped> v_textures;
	std::map<GLuint, t_scoped> v_programs;
	std::map<GLuint, t_scoped> v_shaders;

public:
	static t_session* f_instance()
	{
		if (!v_instance) t_throwable::f_throw(L"must be inside main.");
#ifdef _WIN32
		if (!v_glew) {
			if (glewInit() != GLEW_OK) t_throwable::f_throw(L"glewInit failed.");
			if (GLEW_VERSION_2_0 == GL_FALSE) t_throwable::f_throw(L"GL 2.0 is required.");
			v_glew = true;
		}
#endif
		return v_instance;
	}

	t_session(t_extension* a_extension);
	~t_session();
	t_extension* f_extension() const
	{
		return v_extension;
	}
};

class t_extension : public xemmai::t_extension
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
	t_extension(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
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
		return const_cast<t_extension*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_scoped f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_extension* t_extension::f_extension<t_extension>() const
{
	return this;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_array_of<short>>()
{
	return v_type_array_of_int16;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_array_of<int>>()
{
	return v_type_array_of_int32;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_array_of<float>>()
{
	return v_type_array_of_float32;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_error>()
{
	return v_type_error;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_buffer>()
{
	return v_type_buffer;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_framebuffer>()
{
	return v_type_framebuffer;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_renderbuffer>()
{
	return v_type_renderbuffer;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_texture>()
{
	return v_type_texture;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_program>()
{
	return v_type_program;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_shader>()
{
	return v_type_shader;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_uniform_location>()
{
	return v_type_uniform_location;
}

}

#endif

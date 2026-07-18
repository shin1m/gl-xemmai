#ifndef XEMMAIX__GL__ERROR_H
#define XEMMAIX__GL__ERROR_H

#include "gl.h"

namespace xemmaix::gl
{

class t_error : public t_throwable
{
	GLenum v_error;

public:
	static void f_throw(GLenum a_error);
	static void f_check()
	{
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) f_throw(error);
	}

	t_error(std::wstring_view a_message, GLenum a_error) : t_throwable(a_message), v_error(a_error)
	{
	}
	GLenum f_error() const
	{
		return v_error;
	}
};

template<typename T, std::map<GLuint, t_root> t_session::*A_map>
class t_base_of
{
	friend class t_type_of<T>;
	friend class t_holds<T>;

	template<auto A_new>
	static t_object* f_new(t_type* a_class, auto&&... a_xs)
	{
		auto session = t_session::f_instance();
		auto id = A_new(std::forward<decltype(a_xs)>(a_xs)...);
		t_error::f_check();
		auto p = a_class->f_new<T>();
		p->template f_as<T>().v_entry = (session->*A_map).emplace(id, p).first;
		return p;
	}

	std::map<GLuint, t_root>::iterator v_entry;

protected:
	~t_base_of() = default;

public:
	GLuint f_id() const
	{
		return v_entry->first;
	}
	void f_delete(auto a_delete)
	{
		a_delete(f_id());
		t_error::f_check();
		(t_session::f_instance()->*A_map).erase(v_entry);
		v_entry = {};
	}
};

template<void(*A_do)(GLsizei, GLuint*)>
GLuint f_new1()
{
	GLuint id;
	A_do(1, &id);
	return id;
}

template<void(*A_do)(GLsizei, const GLuint*)>
void f_delete1(GLuint a_id)
{
	A_do(1, &a_id);
}

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_error> : t_uninstantiatable<t_bears<xemmaix::gl::t_error, t_type_of<t_throwable>>>
{
	using t_library = xemmaix::gl::t_library;

	static void f_define(t_library* a_library);

	using t_base::t_base;
};

}

#endif

#ifndef GL__XEMMAI__ERROR_H
#define GL__XEMMAI__ERROR_H

#include "gl.h"

namespace gl
{

namespace xemmai
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

	t_error(const std::wstring& a_message, GLenum a_error) : t_throwable(a_message), v_error(a_error)
	{
	}
	GLenum f_error() const
	{
		return v_error;
	}
};

}

}

namespace xemmai
{

using gl::xemmai::t_error;

template<>
struct t_type_of<t_error> : t_type_of<t_throwable>
{
	typedef gl::xemmai::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	using t_type_of<t_throwable>::t_type_of;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif

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

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_error> : t_uninstantiatable<t_underivable<t_bears<xemmaix::gl::t_error, t_type_of<t_throwable>>>>
{
	typedef xemmaix::gl::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

}

#endif

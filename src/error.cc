#include "error.h"

namespace xemmaix::gl
{

void t_error::f_throw(GLenum a_error)
{
	throw f_new<t_error>(t_session::f_instance()->v_extension, false, L"error"sv, a_error);
}

}

namespace xemmai
{

void t_type_of<xemmaix::gl::t_error>::f_define(t_extension* a_extension)
{
	using namespace xemmaix::gl;
	t_define<t_error, t_throwable>(a_extension, L"Error"sv)
		(L"error"sv, t_member<GLenum(t_error::*)() const, &t_error::f_error>())
	;
}

}

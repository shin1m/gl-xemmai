#include "error.h"

namespace xemmaix::gl
{

void t_error::f_throw(GLenum a_error)
{
	throw t_rvalue(f_new<t_error>(t_session::f_instance()->v_library, L"error"sv, a_error));
}

}

namespace xemmai
{

void t_type_of<xemmaix::gl::t_error>::f_define(t_library* a_library)
{
	using namespace xemmaix::gl;
	t_define{a_library}
		(L"error"sv, t_member<GLenum(t_error::*)() const, &t_error::f_error>())
	.f_derive<t_error, t_throwable>();
}

}

#include "error.h"

namespace xemmaix::gl
{

void t_error::f_throw(GLenum a_error)
{
	t_session* session = t_session::f_instance();
	t_scoped object = t_object::f_allocate(session->v_extension->f_type<t_error>(), false);
	object.f_pointer__(new t_error(L"error"sv, a_error));
	throw object;
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

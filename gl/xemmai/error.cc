#include "error.h"

namespace gl
{

namespace xemmai
{

void t_error::f_throw(GLenum a_error)
{
	t_session* session = t_session::f_instance();
	t_scoped object = t_object::f_allocate(session->v_extension->f_type<t_error>());
	object.f_pointer__(new t_error(L"error", a_error));
	throw object;
}

}

}

namespace xemmai
{

void t_type_of<t_error>::f_define(t_extension* a_extension)
{
	t_define<t_error, t_throwable>(a_extension, L"Error")
		(L"error", t_member<GLenum (t_error::*)() const, &t_error::f_error>())
	;
}

t_type* t_type_of<t_error>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<t_error>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}

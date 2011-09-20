#include "renderbuffer.h"

#include <cassert>

namespace xemmai
{

void t_type_of<t_renderbuffer>::f_define(t_extension* a_extension)
{
	t_define<t_renderbuffer, t_object>(a_extension, L"Renderbuffer")
		(L"delete", t_member<void (t_renderbuffer::*)(), &t_renderbuffer::f_delete>())
	;
}

t_type* t_type_of<t_renderbuffer>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_renderbuffer>::f_finalize(t_object* a_this)
{
	t_renderbuffer* p = static_cast<t_renderbuffer*>(a_this->f_pointer());
	assert(!p);
}

t_transfer t_type_of<t_renderbuffer>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct_with<t_transfer (*)(t_object*), t_renderbuffer::f_construct>::t_bind<t_renderbuffer>::f_do(a_class, a_stack, a_n);
}

}

#include "texture.h"

#include <cassert>

namespace xemmai
{

void t_type_of<t_texture>::f_define(t_extension* a_extension)
{
	t_define<t_texture, t_object>(a_extension, L"Texture")
		(L"delete", t_member<void (t_texture::*)(), &t_texture::f_delete>())
	;
}

t_type* t_type_of<t_texture>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<t_texture>::f_finalize(t_object* a_this)
{
	t_texture* p = static_cast<t_texture*>(a_this->f_pointer());
	assert(!p);
}

t_scoped t_type_of<t_texture>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped (*)(t_object*), t_texture::f_construct>::t_bind<t_texture>::f_do(a_class, a_stack, a_n);
}

}

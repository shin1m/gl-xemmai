#include "texture.h"

namespace xemmai
{

using namespace xemmaix::gl;

void t_type_of<t_texture>::f_define(t_library* a_library)
{
	t_define{a_library}
	(L"delete"sv, t_member<void(t_texture::*)(), &t_texture::f_delete>())
	.f_derive<t_texture, t_object>();
}

t_pvalue t_type_of<t_texture>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_object*(*)(t_type*), t_texture::f_new<f_new1<glGenTextures>>>::f_do(this, a_stack, a_n);
}

}

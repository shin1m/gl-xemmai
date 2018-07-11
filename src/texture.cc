#include "texture.h"

namespace xemmai
{

void t_type_of<xemmaix::gl::t_texture>::f_define(t_extension* a_extension)
{
	using namespace xemmaix::gl;
	t_define<t_texture, t_object>(a_extension, L"Texture")
		(L"delete", t_member<void(t_texture::*)(), &t_texture::f_delete>())
	;
}

t_scoped t_type_of<xemmaix::gl::t_texture>::f_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*), xemmaix::gl::t_texture::f_construct>::t_bind<xemmaix::gl::t_texture>::f_do(this, a_stack, a_n);
}

}

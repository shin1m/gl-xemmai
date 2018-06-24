#include "framebuffer.h"

#include <cassert>

namespace xemmai
{

void t_type_of<xemmaix::gl::t_framebuffer>::f_define(t_extension* a_extension)
{
	using namespace xemmaix::gl;
	t_define<t_framebuffer, t_object>(a_extension, L"Framebuffer")
		(L"delete", t_member<void(t_framebuffer::*)(), &t_framebuffer::f_delete>())
	;
}

t_type* t_type_of<xemmaix::gl::t_framebuffer>::f_derive()
{
	return nullptr;
}

void t_type_of<xemmaix::gl::t_framebuffer>::f_finalize(t_object* a_this)
{
	auto p = static_cast<xemmaix::gl::t_framebuffer*>(a_this->f_pointer());
	assert(!p);
}

t_scoped t_type_of<xemmaix::gl::t_framebuffer>::f_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*), xemmaix::gl::t_framebuffer::f_construct>::t_bind<xemmaix::gl::t_framebuffer>::f_do(this, a_stack, a_n);
}

}

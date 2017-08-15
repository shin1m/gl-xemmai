#include "buffer.h"

#include <cassert>

namespace xemmai
{

void t_type_of<xemmaix::gl::t_buffer>::f_define(t_extension* a_extension)
{
	using namespace xemmaix::gl;
	t_define<t_buffer, t_object>(a_extension, L"Buffer")
		(L"delete", t_member<void(t_buffer::*)(), &t_buffer::f_delete>())
	;
}

t_type* t_type_of<xemmaix::gl::t_buffer>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<xemmaix::gl::t_buffer>::f_finalize(t_object* a_this)
{
	auto p = static_cast<xemmaix::gl::t_buffer*>(a_this->f_pointer());
	assert(!p);
}

t_scoped t_type_of<xemmaix::gl::t_buffer>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_object*), xemmaix::gl::t_buffer::f_construct>::t_bind<xemmaix::gl::t_buffer>::f_do(a_class, a_stack, a_n);
}

}

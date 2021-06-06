#include "renderbuffer.h"

namespace xemmai
{

void t_type_of<xemmaix::gl::t_renderbuffer>::f_define(t_library* a_library)
{
	using namespace xemmaix::gl;
	t_define{a_library}
		(L"delete"sv, t_member<void(t_renderbuffer::*)(), &t_renderbuffer::f_delete>())
	.f_derive<t_renderbuffer, t_object>();
}

t_pvalue t_type_of<xemmaix::gl::t_renderbuffer>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*), xemmaix::gl::t_renderbuffer::f_construct>::t_bind<xemmaix::gl::t_renderbuffer>::f_do(this, a_stack, a_n);
}

}

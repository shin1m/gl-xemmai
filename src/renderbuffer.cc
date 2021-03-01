#include "renderbuffer.h"

namespace xemmai
{

void t_type_of<xemmaix::gl::t_renderbuffer>::f_define(t_extension* a_extension)
{
	using namespace xemmaix::gl;
	t_define<t_renderbuffer, t_object>(a_extension, L"Renderbuffer"sv)
		(L"delete"sv, t_member<void(t_renderbuffer::*)(), &t_renderbuffer::f_delete>())
	;
}

t_pvalue t_type_of<xemmaix::gl::t_renderbuffer>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*), xemmaix::gl::t_renderbuffer::f_construct>::t_bind<xemmaix::gl::t_renderbuffer>::f_do(this, a_stack, a_n);
}

}

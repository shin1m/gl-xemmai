#ifndef XEMMAIX__GL__RENDERBUFFER_H
#define XEMMAIX__GL__RENDERBUFFER_H

#include "error.h"

namespace xemmaix::gl
{

struct t_renderbuffer : t_base_of<t_renderbuffer, &t_session::v_renderbuffers>
{
	void f_delete()
	{
		t_base_of::f_delete(f_delete1<glDeleteRenderbuffers>);
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_renderbuffer> : xemmaix::gl::t_holds<xemmaix::gl::t_renderbuffer>
{
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif

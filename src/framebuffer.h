#ifndef XEMMAIX__GL__FRAMEBUFFER_H
#define XEMMAIX__GL__FRAMEBUFFER_H

#include "error.h"

namespace xemmaix::gl
{

struct t_framebuffer : t_base_of<t_framebuffer, &t_session::v_framebuffers>
{
	void f_delete()
	{
		t_base_of::f_delete(f_delete1<glDeleteFramebuffers>);
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_framebuffer> : xemmaix::gl::t_holds<xemmaix::gl::t_framebuffer>
{
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif

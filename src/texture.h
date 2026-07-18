#ifndef XEMMAIX__GL__TEXTURE_H
#define XEMMAIX__GL__TEXTURE_H

#include "error.h"

namespace xemmaix::gl
{

struct t_texture : t_base_of<t_texture, &t_session::v_textures>
{
	void f_delete()
	{
		t_base_of::f_delete(f_delete1<glDeleteTextures>);
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_texture> : xemmaix::gl::t_holds<xemmaix::gl::t_texture>
{
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif

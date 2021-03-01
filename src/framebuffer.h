#ifndef XEMMAIX__GL__FRAMEBUFFER_H
#define XEMMAIX__GL__FRAMEBUFFER_H

#include "error.h"

namespace xemmaix::gl
{

class t_framebuffer
{
	friend class t_type_of<t_object>;
	friend class t_holds<t_framebuffer>;

	std::map<GLuint, t_root>::iterator v_entry;

	t_framebuffer(t_session* a_session, GLuint a_id) : v_entry(a_session->v_framebuffers.emplace(a_id, t_object::f_of(this)).first)
	{
	}
	~t_framebuffer() = default;

public:
	static t_pvalue f_construct(t_type* a_class)
	{
		auto session = t_session::f_instance();
		GLuint id;
		glGenFramebuffers(1, &id);
		t_error::f_check();
		return a_class->f_new<t_framebuffer>(false, session, id);
	}

	GLuint f_id() const
	{
		return v_entry->first;
	}
	void f_delete()
	{
		glDeleteFramebuffers(1, &v_entry->first);
		t_error::f_check();
		t_session::f_instance()->v_framebuffers.erase(v_entry);
		v_entry = {};
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_framebuffer> : xemmaix::gl::t_holds<xemmaix::gl::t_framebuffer>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif

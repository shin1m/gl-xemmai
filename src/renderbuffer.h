#ifndef XEMMAIX__GL__RENDERBUFFER_H
#define XEMMAIX__GL__RENDERBUFFER_H

#include "error.h"

namespace xemmaix::gl
{

class t_renderbuffer
{
	friend class t_type_of<t_object>;
	friend class t_holds<t_renderbuffer>;

	std::map<GLuint, t_scoped>::iterator v_entry;

	t_renderbuffer(t_session* a_session, GLuint a_id) : v_entry(a_session->v_renderbuffers.emplace(a_id, t_object::f_of(this)).first)
	{
	}
	~t_renderbuffer() = default;

public:
	static t_scoped f_construct(t_type* a_class)
	{
		auto session = t_session::f_instance();
		GLuint id;
		glGenRenderbuffers(1, &id);
		t_error::f_check();
		return a_class->f_new<t_renderbuffer>(false, session, id);
	}

	GLuint f_id() const
	{
		return v_entry->first;
	}
	void f_delete()
	{
		glDeleteRenderbuffers(1, &v_entry->first);
		t_error::f_check();
		t_session::f_instance()->v_renderbuffers.erase(v_entry);
		v_entry = {};
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_renderbuffer> : xemmaix::gl::t_holds<xemmaix::gl::t_renderbuffer>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif

#ifndef XEMMAIX__GL__FRAMEBUFFER_H
#define XEMMAIX__GL__FRAMEBUFFER_H

#include "error.h"

namespace xemmaix::gl
{

class t_framebuffer
{
	std::map<GLuint, t_scoped>::iterator v_entry;

	t_framebuffer(std::map<GLuint, t_scoped>::iterator a_entry) : v_entry(a_entry)
	{
	}
	~t_framebuffer()
	{
		v_entry->second.f_pointer__(nullptr);
		t_session* session = t_session::f_instance();
		session->v_framebuffers.erase(v_entry);
	}

public:
	static t_scoped f_construct(t_object* a_class)
	{
		t_session* session = t_session::f_instance();
		GLuint id;
		glGenFramebuffers(1, &id);
		t_error::f_check();
		t_scoped object = t_object::f_allocate(a_class);
		object.f_pointer__(new t_framebuffer(session->v_framebuffers.insert(std::make_pair(id, static_cast<t_object*>(object))).first));
		return object;
	}

	GLuint f_id() const
	{
		return v_entry->first;
	}
	void f_delete()
	{
		glDeleteFramebuffers(1, &v_entry->first);
		t_error::f_check();
		delete this;
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_framebuffer> : t_type
{
#include "cast.h"
	typedef xemmaix::gl::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

#endif

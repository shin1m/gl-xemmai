#ifndef GL__XEMMAI__RENDERBUFFER_H
#define GL__XEMMAI__RENDERBUFFER_H

#include "error.h"

namespace gl
{

namespace xemmai
{

class t_renderbuffer
{
	std::map<GLuint, t_scoped>::iterator v_entry;

	t_renderbuffer(std::map<GLuint, t_scoped>::iterator a_entry) : v_entry(a_entry)
	{
	}
	~t_renderbuffer()
	{
		v_entry->second.f_pointer__(nullptr);
		t_session* session = t_session::f_instance();
		session->v_renderbuffers.erase(v_entry);
	}

public:
	static t_scoped f_construct(t_object* a_class)
	{
		t_session* session = t_session::f_instance();
		GLuint id;
		glGenRenderbuffers(1, &id);
		t_error::f_check();
		t_scoped object = t_object::f_allocate(a_class);
		object.f_pointer__(new t_renderbuffer(session->v_renderbuffers.insert(std::make_pair(id, static_cast<t_object*>(object))).first));
		return object;
	}

	GLuint f_id() const
	{
		return v_entry->first;
	}
	void f_delete()
	{
		glDeleteRenderbuffers(1, &v_entry->first);
		t_error::f_check();
		delete this;
	}
};

}

}

namespace xemmai
{

using gl::xemmai::t_renderbuffer;

template<>
struct t_type_of<t_renderbuffer> : t_type
{
#include "cast.h"
	typedef gl::xemmai::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_scoped* a_stack, size_t a_n);
};

}

#endif

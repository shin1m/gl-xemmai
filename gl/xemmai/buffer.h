#ifndef GL__XEMMAI__BUFFER_H
#define GL__XEMMAI__BUFFER_H

#include "error.h"

namespace gl
{

namespace xemmai
{

class t_buffer
{
	std::map<GLuint, t_scoped>::iterator v_entry;

	t_buffer(std::map<GLuint, t_scoped>::iterator a_entry) : v_entry(a_entry)
	{
	}
	~t_buffer()
	{
		v_entry->second.f_pointer__(0);
		t_session* session = t_session::f_instance();
		session->v_buffers.erase(v_entry);
	}

public:
	static t_transfer f_construct(t_object* a_class)
	{
		t_session* session = t_session::f_instance();
		GLuint id;
		glGenBuffers(1, &id);
		t_error::f_check();
		t_transfer object = t_object::f_allocate(a_class);
		object.f_pointer__(new t_buffer(session->v_buffers.insert(std::make_pair(id, static_cast<t_object*>(object))).first));
		return object;
	}

	GLuint f_id() const
	{
		return v_entry->first;
	}
	void f_delete()
	{
		glDeleteBuffers(1, &v_entry->first);
		t_error::f_check();
		delete this;
	}
};

}

}

namespace xemmai
{

using gl::xemmai::t_buffer;

template<>
struct t_type_of<t_buffer> : t_type
{
#include "cast.h"
	typedef gl::xemmai::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual t_transfer f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif

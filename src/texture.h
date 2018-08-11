#ifndef XEMMAIX__GL__TEXTURE_H
#define XEMMAIX__GL__TEXTURE_H

#include "error.h"

namespace xemmaix::gl
{

class t_texture
{
	std::map<GLuint, t_scoped>::iterator v_entry;

	t_texture(std::map<GLuint, t_scoped>::iterator a_entry) : v_entry(a_entry)
	{
	}
	~t_texture()
	{
		v_entry->second.f_pointer__(nullptr);
		t_session* session = t_session::f_instance();
		session->v_textures.erase(v_entry);
	}

public:
	static t_scoped f_construct(t_type* a_class)
	{
		t_session* session = t_session::f_instance();
		GLuint id;
		glGenTextures(1, &id);
		t_error::f_check();
		t_scoped object = t_object::f_allocate(a_class);
		object.f_pointer__(new t_texture(session->v_textures.insert(std::make_pair(id, static_cast<t_object*>(object))).first));
		return object;
	}

	GLuint f_id() const
	{
		return v_entry->first;
	}
	void f_delete()
	{
		glDeleteTextures(1, &v_entry->first);
		t_error::f_check();
		delete this;
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_texture> : xemmaix::gl::t_holds<xemmaix::gl::t_texture>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif

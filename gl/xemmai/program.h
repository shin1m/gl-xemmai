#ifndef GL__XEMMAI__PROGRAM_H
#define GL__XEMMAI__PROGRAM_H

#include "shader.h"
#include "uniform_location.h"

namespace gl
{

namespace xemmai
{

class t_program
{
	std::map<GLuint, t_scoped>::iterator v_entry;

	t_program(std::map<GLuint, t_scoped>::iterator a_entry) : v_entry(a_entry)
	{
	}
	~t_program()
	{
		v_entry->second.f_pointer__(nullptr);
		t_session* session = t_session::f_instance();
		session->v_programs.erase(v_entry);
	}

public:
	static t_scoped f_construct(t_object* a_class)
	{
		t_session* session = t_session::f_instance();
		GLuint id = glCreateProgram();
		t_error::f_check();
		t_scoped object = t_object::f_allocate(a_class);
		object.f_pointer__(new t_program(session->v_programs.insert(std::make_pair(id, static_cast<t_object*>(object))).first));
		return object;
	}

	GLuint f_id() const
	{
		return v_entry->first;
	}
	void f_delete()
	{
		glDeleteProgram(f_id());
		t_error::f_check();
		delete this;
	}
	void f_attach_shader(const t_shader& a_shader)
	{
		glAttachShader(f_id(), a_shader.f_id());
		t_error::f_check();
	}
	void f_bind_attrib_location(GLuint a_index, const std::wstring& a_name)
	{
		glBindAttribLocation(f_id(), a_index, f_convert(a_name).c_str());
		t_error::f_check();
	}
	void f_detach_shader(const t_shader& a_shader)
	{
		glDetachShader(f_id(), a_shader.f_id());
		t_error::f_check();
	}
	t_scoped f_get_active_attrib(GLuint a_index) const
	{
		GLint n = f_get_parameteri(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
		GLint size;
		GLenum type;
		std::vector<GLchar> name(n);
		glGetActiveAttrib(f_id(), a_index, n, NULL, &size, &type, &name[0]);
		return f_tuple(t_scoped(size), t_scoped(type), f_global()->f_as(f_convert(&name[0])));
	}
	t_scoped f_get_active_uniform(GLuint a_index) const
	{
		GLint n = f_get_parameteri(GL_ACTIVE_UNIFORM_MAX_LENGTH);
		GLint size;
		GLenum type;
		std::vector<GLchar> name(n);
		glGetActiveUniform(f_id(), a_index, n, NULL, &size, &type, &name[0]);
		return f_tuple(t_scoped(size), t_scoped(type), f_global()->f_as(f_convert(&name[0])));
	}
	t_scoped f_get_attached_shaders() const;
	GLint f_get_attrib_location(const std::wstring& a_name) const
	{
		GLint index = glGetAttribLocation(f_id(), f_convert(a_name).c_str());
		if (index < 0) t_error::f_throw(glGetError());
		return index;
	}
	GLint f_get_parameteri(GLenum a_name) const
	{
		GLint value = 0;
		glGetProgramiv(f_id(), a_name, &value);
		return value;
	}
	std::wstring f_get_info_log() const
	{
		GLint n = f_get_parameteri(GL_INFO_LOG_LENGTH);
		std::vector<GLchar> log(n);
		glGetProgramInfoLog(f_id(), n, NULL, &log[0]);
		return f_convert(&log[0]);
	}
	t_scoped f_get_uniformfv(const t_uniform_location& a_location) const;
	t_scoped f_get_uniformiv(const t_uniform_location& a_location) const;
	t_uniform_location f_get_uniform_location(const std::wstring& a_name) const
	{
		GLint index = glGetUniformLocation(f_id(), f_convert(a_name).c_str());
		if (index < 0) t_error::f_throw(glGetError());
		return index;
	}
	void f_link()
	{
		glLinkProgram(f_id());
	}
	void f_validate()
	{
		glValidateProgram(f_id());
	}
};

}

}

namespace xemmai
{

using gl::xemmai::t_program;

template<>
struct t_type_of<t_program> : t_type
{
#include "cast.h"
	typedef gl::xemmai::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

#endif

#ifndef XEMMAIX__GL__PROGRAM_H
#define XEMMAIX__GL__PROGRAM_H

#include "shader.h"
#include "uniform_location.h"

namespace xemmaix::gl
{

class t_program
{
	friend class t_type_of<t_object>;
	friend class t_holds<t_program>;

	std::map<GLuint, t_root>::iterator v_entry;

	t_program(t_session* a_session, GLuint a_id) : v_entry(a_session->v_programs.emplace(a_id, t_object::f_of(this)).first)
	{
	}
	~t_program() = default;

public:
	static t_pvalue f_construct(t_type* a_class)
	{
		auto session = t_session::f_instance();
		GLuint id = glCreateProgram();
		t_error::f_check();
		return a_class->f_new<t_program>(session, id);
	}

	GLuint f_id() const
	{
		return v_entry->first;
	}
	void f_delete()
	{
		glDeleteProgram(f_id());
		t_error::f_check();
		t_session::f_instance()->v_programs.erase(v_entry);
		v_entry = {};
	}
	void f_attach_shader(const t_shader& a_shader)
	{
		glAttachShader(f_id(), a_shader.f_id());
		t_error::f_check();
	}
	void f_bind_attrib_location(GLuint a_index, std::wstring_view a_name)
	{
		glBindAttribLocation(f_id(), a_index, f_convert(a_name).c_str());
		t_error::f_check();
	}
	void f_detach_shader(const t_shader& a_shader)
	{
		glDetachShader(f_id(), a_shader.f_id());
		t_error::f_check();
	}
	t_pvalue f_get_active_attrib(GLuint a_index) const
	{
		GLint n = f_get_parameteri(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH);
		GLint size;
		GLenum type;
		std::vector<GLchar> name(n);
		glGetActiveAttrib(f_id(), a_index, n, NULL, &size, &type, &name[0]);
		return f_tuple(size, type, f_global()->f_as(f_convert(&name[0])));
	}
	t_pvalue f_get_active_uniform(GLuint a_index) const
	{
		GLint n = f_get_parameteri(GL_ACTIVE_UNIFORM_MAX_LENGTH);
		GLint size;
		GLenum type;
		std::vector<GLchar> name(n);
		glGetActiveUniform(f_id(), a_index, n, NULL, &size, &type, &name[0]);
		return f_tuple(size, type, f_global()->f_as(f_convert(&name[0])));
	}
	t_pvalue f_get_attached_shaders() const;
	GLint f_get_attrib_location(std::wstring_view a_name) const
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
	t_pvalue f_get_uniformfv(const t_uniform_location& a_location) const;
	t_pvalue f_get_uniformiv(const t_uniform_location& a_location) const;
	t_uniform_location f_get_uniform_location(std::wstring_view a_name) const
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

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_program> : xemmaix::gl::t_holds<xemmaix::gl::t_program>
{
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif

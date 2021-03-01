#ifndef XEMMAIX__GL__SHADER_H
#define XEMMAIX__GL__SHADER_H

#include "error.h"

namespace xemmaix::gl
{

class t_shader
{
	friend class t_type_of<t_object>;
	friend class t_holds<t_shader>;

	std::map<GLuint, t_root>::iterator v_entry;

	t_shader(t_session* a_session, GLuint a_id) : v_entry(a_session->v_shaders.emplace(a_id, t_object::f_of(this)).first)
	{
	}
	~t_shader() = default;

public:
	static t_pvalue f_construct(t_type* a_class, GLenum a_type)
	{
		auto session = t_session::f_instance();
		GLuint id = glCreateShader(a_type);
		t_error::f_check();
		return a_class->f_new<t_shader>(false, session, id);
	}

	GLuint f_id() const
	{
		return v_entry->first;
	}
	void f_delete()
	{
		glDeleteShader(f_id());
		t_error::f_check();
		t_session::f_instance()->v_shaders.erase(v_entry);
		v_entry = {};
	}
	void f_compile()
	{
		glCompileShader(f_id());
	}
	GLint f_get_parameteri(GLenum a_name) const
	{
		GLint value;
		glGetShaderiv(f_id(), a_name, &value);
		return value;
	}
	std::wstring f_get_info_log() const
	{
		GLint n = f_get_parameteri(GL_INFO_LOG_LENGTH);
		std::vector<GLchar> log(n);
		glGetShaderInfoLog(f_id(), n, NULL, &log[0]);
		return f_convert(&log[0]);
	}
	std::wstring f_get_source() const
	{
		GLint n = f_get_parameteri(GL_SHADER_SOURCE_LENGTH);
		std::vector<GLchar> log(n);
		glGetShaderSource(f_id(), n, NULL, &log[0]);
		return f_convert(&log[0]);
	}
	void f_source(std::wstring_view a_source)
	{
		std::string s = f_convert(a_source);
		const GLchar* p = s.c_str();
		glShaderSource(f_id(), 1, &p, NULL);
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_shader> : xemmaix::gl::t_holds<xemmaix::gl::t_shader>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif

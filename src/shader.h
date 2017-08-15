#ifndef XEMMAIX__GL__SHADER_H
#define XEMMAIX__GL__SHADER_H

#include "error.h"

namespace xemmaix
{

namespace gl
{

class t_shader
{
	std::map<GLuint, t_scoped>::iterator v_entry;

	t_shader(std::map<GLuint, t_scoped>::iterator a_entry) : v_entry(a_entry)
	{
	}
	~t_shader()
	{
		v_entry->second.f_pointer__(nullptr);
		t_session* session = t_session::f_instance();
		session->v_shaders.erase(v_entry);
	}

public:
	static t_scoped f_construct(t_object* a_class, GLenum a_type)
	{
		t_session* session = t_session::f_instance();
		GLuint id = glCreateShader(a_type);
		t_error::f_check();
		t_scoped object = t_object::f_allocate(a_class);
		object.f_pointer__(new t_shader(session->v_shaders.insert(std::make_pair(id, static_cast<t_object*>(object))).first));
		return object;
	}

	GLuint f_id() const
	{
		return v_entry->first;
	}
	void f_delete()
	{
		glDeleteShader(f_id());
		t_error::f_check();
		delete this;
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
	void f_source(const std::wstring& a_source)
	{
		std::string s = f_convert(a_source);
		const GLchar* p = s.c_str();
		glShaderSource(f_id(), 1, &p, NULL);
	}
};

}

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_shader> : t_type
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

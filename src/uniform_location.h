#ifndef XEMMAIX__GL__UNIFORM_LOCATION_H
#define XEMMAIX__GL__UNIFORM_LOCATION_H

#include "error.h"

namespace xemmaix::gl
{

class t_uniform_location
{
	GLint v_id;

public:
	t_uniform_location(GLint a_id) : v_id(a_id)
	{
	}
	GLint f_id() const
	{
		return v_id;
	}
	void f_uniform1f(GLfloat a_x)
	{
		glUniform1f(v_id, a_x);
		t_error::f_check();
	}
	void f_uniform1fv(const t_bytes& a_values)
	{
		glUniform1fv(v_id, a_values.f_size() / sizeof(GLfloat), reinterpret_cast<const GLfloat*>(&a_values[0]));
		t_error::f_check();
	}
	void f_uniform1i(GLint a_x)
	{
		glUniform1i(v_id, a_x);
		t_error::f_check();
	}
	void f_uniform1iv(const t_bytes& a_values)
	{
		glUniform1iv(v_id, a_values.f_size() / sizeof(GLint), reinterpret_cast<const GLint*>(&a_values[0]));
		t_error::f_check();
	}
	void f_uniform2f(GLfloat a_x, GLfloat a_y)
	{
		glUniform2f(v_id, a_x, a_y);
		t_error::f_check();
	}
	void f_uniform2fv(const t_bytes& a_values)
	{
		glUniform2fv(v_id, a_values.f_size() / (sizeof(GLfloat) * 2), reinterpret_cast<const GLfloat*>(&a_values[0]));
		t_error::f_check();
	}
	void f_uniform2i(GLint a_x, GLint a_y)
	{
		glUniform2i(v_id, a_x, a_y);
		t_error::f_check();
	}
	void f_uniform2iv(const t_bytes& a_values)
	{
		glUniform2iv(v_id, a_values.f_size() / (sizeof(GLint) * 2), reinterpret_cast<const GLint*>(&a_values[0]));
		t_error::f_check();
	}
	void f_uniform3f(GLfloat a_x, GLfloat a_y, GLfloat a_z)
	{
		glUniform3f(v_id, a_x, a_y, a_z);
		t_error::f_check();
	}
	void f_uniform3fv(const t_bytes& a_values)
	{
		glUniform3fv(v_id, a_values.f_size() / (sizeof(GLfloat) * 3), reinterpret_cast<const GLfloat*>(&a_values[0]));
		t_error::f_check();
	}
	void f_uniform3i(GLint a_x, GLint a_y, GLint a_z)
	{
		glUniform3i(v_id, a_x, a_y, a_z);
		t_error::f_check();
	}
	void f_uniform3iv(const t_bytes& a_values)
	{
		glUniform3iv(v_id, a_values.f_size() / (sizeof(GLint) * 3), reinterpret_cast<const GLint*>(&a_values[0]));
		t_error::f_check();
	}
	void f_uniform4f(GLfloat a_x, GLfloat a_y, GLfloat a_z, GLfloat a_w)
	{
		glUniform4f(v_id, a_x, a_y, a_z, a_w);
		t_error::f_check();
	}
	void f_uniform4fv(const t_bytes& a_values)
	{
		glUniform4fv(v_id, a_values.f_size() / (sizeof(GLfloat) * 4), reinterpret_cast<const GLfloat*>(&a_values[0]));
		t_error::f_check();
	}
	void f_uniform4i(GLint a_x, GLint a_y, GLint a_z, GLint a_w)
	{
		glUniform4i(v_id, a_x, a_y, a_z, a_w);
		t_error::f_check();
	}
	void f_uniform4iv(const t_bytes& a_values)
	{
		glUniform4iv(v_id, a_values.f_size() / (sizeof(GLint) * 4), reinterpret_cast<const GLint*>(&a_values[0]));
		t_error::f_check();
	}
	void f_matrix2fv(bool a_transpose, const t_bytes& a_values)
	{
		glUniformMatrix2fv(v_id, a_values.f_size() / (sizeof(GLfloat) * 4), a_transpose ? GL_TRUE : GL_FALSE, reinterpret_cast<const GLfloat*>(&a_values[0]));
		t_error::f_check();
	}
	void f_matrix3fv(bool a_transpose, const t_bytes& a_values)
	{
		glUniformMatrix3fv(v_id, a_values.f_size() / (sizeof(GLfloat) * 9), a_transpose ? GL_TRUE : GL_FALSE, reinterpret_cast<const GLfloat*>(&a_values[0]));
		t_error::f_check();
	}
	void f_matrix4fv(bool a_transpose, const t_bytes& a_values)
	{
		glUniformMatrix4fv(v_id, a_values.f_size() / (sizeof(GLfloat) * 16), a_transpose ? GL_TRUE : GL_FALSE, reinterpret_cast<const GLfloat*>(&a_values[0]));
		t_error::f_check();
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::gl::t_uniform_location> : t_uninstantiatable<t_holds<xemmaix::gl::t_uniform_location>>
{
	using t_library = xemmaix::gl::t_library;

	static t_pvalue f_transfer(const t_library* a_library, auto&& a_value)
	{
		return xemmai::f_new<xemmaix::gl::t_uniform_location>(a_library, std::forward<decltype(a_value)>(a_value));
	}
	static void f_define(t_library* a_library);

	using t_base::t_base;
};

}

#endif

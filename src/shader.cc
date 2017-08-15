#include "shader.h"

#include <cassert>

namespace xemmai
{

void t_type_of<xemmaix::gl::t_shader>::f_define(t_extension* a_extension)
{
	using namespace xemmaix::gl;
	t_define<t_shader, t_object>(a_extension, L"Shader")
		(L"delete", t_member<void(t_shader::*)(), &t_shader::f_delete>())
		(L"compile", t_member<void(t_shader::*)(), &t_shader::f_compile>())
		(L"get_parameteri", t_member<GLint(t_shader::*)(GLenum) const, &t_shader::f_get_parameteri>())
		(L"get_info_log", t_member<std::wstring(t_shader::*)() const, &t_shader::f_get_info_log>())
		(L"get_source", t_member<std::wstring(t_shader::*)() const, &t_shader::f_get_source>())
		(L"source", t_member<void(t_shader::*)(const std::wstring&), &t_shader::f_source>())
	;
}

t_type* t_type_of<xemmaix::gl::t_shader>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<xemmaix::gl::t_shader>::f_finalize(t_object* a_this)
{
	auto p = static_cast<xemmaix::gl::t_shader*>(a_this->f_pointer());
	assert(!p);
}

t_scoped t_type_of<xemmaix::gl::t_shader>::f_construct(t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_object*, GLenum), xemmaix::gl::t_shader::f_construct>::t_bind<xemmaix::gl::t_shader>::f_do(a_class, a_stack, a_n);
}

}

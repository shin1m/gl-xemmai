#include "shader.h"

namespace xemmai
{

using namespace xemmaix::gl;

void t_type_of<t_shader>::f_define(t_library* a_library)
{
	t_define{a_library}
	(L"delete"sv, t_member<void(t_shader::*)(), &t_shader::f_delete>())
	(L"compile"sv, t_member<void(t_shader::*)(), &t_shader::f_compile>())
	(L"get_parameteri"sv, t_member<GLint(t_shader::*)(GLenum) const, &t_shader::f_get_parameteri>())
	(L"get_info_log"sv, t_member<std::wstring(t_shader::*)() const, &t_shader::f_get_info_log>())
	(L"get_source"sv, t_member<std::wstring(t_shader::*)() const, &t_shader::f_get_source>())
	(L"source"sv, t_member<void(t_shader::*)(std::wstring_view), &t_shader::f_source>())
	.f_derive<t_shader, t_object>();
}

t_pvalue t_type_of<t_shader>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_object*(*)(t_type*, GLenum&&), t_shader::f_new<glCreateShader>>::f_do(this, a_stack, a_n);
}

}

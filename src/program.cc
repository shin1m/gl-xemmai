#include "program.h"

namespace xemmaix::gl
{

t_pvalue t_program::f_get_attached_shaders() const
{
	auto session = t_session::f_instance();
	GLint n = f_get_parameteri(GL_ATTACHED_SHADERS);
	std::vector<GLuint> shaders(n);
	glGetAttachedShaders(f_id(), n, NULL, &shaders[0]);
	return t_tuple::f_instantiate(n, [&](auto& tuple)
	{
		for (GLint i = 0; i < n; ++i) new(&tuple[i]) t_svalue(session->v_shaders.find(shaders[i])->second);
	});
}

t_pvalue t_program::f_get_uniformfv(const t_uniform_location& a_location) const
{
	auto p = t_bytes::f_instantiate(sizeof(GLfloat) * 16);
	auto& bytes = f_as<t_bytes&>(p);
	glGetUniformfv(f_id(), a_location.f_id(), reinterpret_cast<GLfloat*>(&bytes[0]));
	return p;
}

t_pvalue t_program::f_get_uniformiv(const t_uniform_location& a_location) const
{
	auto p = t_bytes::f_instantiate(sizeof(GLint) * 16);
	auto& bytes = f_as<t_bytes&>(p);
	glGetUniformiv(f_id(), a_location.f_id(), reinterpret_cast<GLint*>(&bytes[0]));
	return p;
}

}

namespace xemmai
{

void t_type_of<xemmaix::gl::t_program>::f_define(t_library* a_library)
{
	using namespace xemmaix::gl;
	t_define{a_library}
	(L"delete"sv, t_member<void(t_program::*)(), &t_program::f_delete>())
	(L"attach_shader"sv, t_member<void(t_program::*)(const t_shader&), &t_program::f_attach_shader>())
	(L"bind_attrib_location"sv, t_member<void(t_program::*)(GLuint, std::wstring_view), &t_program::f_bind_attrib_location>())
	(L"detach_shader"sv, t_member<void(t_program::*)(const t_shader&), &t_program::f_detach_shader>())
	(L"get_active_attrib"sv, t_member<t_pvalue(t_program::*)(GLuint) const, &t_program::f_get_active_attrib>())
	(L"get_active_uniform"sv, t_member<t_pvalue(t_program::*)(GLuint) const, &t_program::f_get_active_uniform>())
	(L"get_attached_shaders"sv, t_member<t_pvalue(t_program::*)() const, &t_program::f_get_attached_shaders>())
	(L"get_attrib_location"sv, t_member<GLint(t_program::*)(std::wstring_view) const, &t_program::f_get_attrib_location>())
	(L"get_parameteri"sv, t_member<GLint(t_program::*)(GLenum) const, &t_program::f_get_parameteri>())
	(L"get_info_log"sv, t_member<std::wstring(t_program::*)() const, &t_program::f_get_info_log>())
	(L"get_uniformfv"sv, t_member<t_pvalue(t_program::*)(const t_uniform_location&) const, &t_program::f_get_uniformfv>())
	(L"get_uniformiv"sv, t_member<t_pvalue(t_program::*)(const t_uniform_location&) const, &t_program::f_get_uniformiv>())
	(L"get_uniform_location"sv, t_member<t_uniform_location(t_program::*)(std::wstring_view) const, &t_program::f_get_uniform_location>())
	(L"link"sv, t_member<void(t_program::*)(), &t_program::f_link>())
	(L"validate"sv, t_member<void(t_program::*)(), &t_program::f_validate>())
	.f_derive<t_program, t_object>();
}

t_pvalue t_type_of<xemmaix::gl::t_program>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*), xemmaix::gl::t_program::f_construct>::t_bind<xemmaix::gl::t_program>::f_do(this, a_stack, a_n);
}

}

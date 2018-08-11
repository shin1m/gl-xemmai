#include "program.h"

namespace xemmaix::gl
{

t_scoped t_program::f_get_attached_shaders() const
{
	t_session* session = t_session::f_instance();
	GLint n = f_get_parameteri(GL_ATTACHED_SHADERS);
	std::vector<GLuint> shaders(n);
	glGetAttachedShaders(f_id(), n, NULL, &shaders[0]);
	t_scoped p = t_tuple::f_instantiate(n);
	t_tuple& tuple = f_as<t_tuple&>(p);
	for (GLint i = 0; i < n; ++i) tuple[i].f_construct(session->v_shaders.find(shaders[i])->second);
	return p;
}

t_scoped t_program::f_get_uniformfv(const t_uniform_location& a_location) const
{
	t_scoped p = t_bytes::f_instantiate(sizeof(GLfloat) * 16);
	t_bytes& bytes = f_as<t_bytes&>(p);
	glGetUniformfv(f_id(), a_location.f_id(), reinterpret_cast<GLfloat*>(&bytes[0]));
	return p;
}

t_scoped t_program::f_get_uniformiv(const t_uniform_location& a_location) const
{
	t_scoped p = t_bytes::f_instantiate(sizeof(GLint) * 16);
	t_bytes& bytes = f_as<t_bytes&>(p);
	glGetUniformiv(f_id(), a_location.f_id(), reinterpret_cast<GLint*>(&bytes[0]));
	return p;
}

}

namespace xemmai
{

void t_type_of<xemmaix::gl::t_program>::f_define(t_extension* a_extension)
{
	using namespace xemmaix::gl;
	t_define<t_program, t_object>(a_extension, L"Program")
		(L"delete", t_member<void(t_program::*)(), &t_program::f_delete>())
		(L"attach_shader", t_member<void(t_program::*)(const t_shader&), &t_program::f_attach_shader>())
		(L"bind_attrib_location", t_member<void(t_program::*)(GLuint, const std::wstring&), &t_program::f_bind_attrib_location>())
		(L"detach_shader", t_member<void(t_program::*)(const t_shader&), &t_program::f_detach_shader>())
		(L"get_active_attrib", t_member<t_scoped(t_program::*)(GLuint) const, &t_program::f_get_active_attrib>())
		(L"get_active_uniform", t_member<t_scoped(t_program::*)(GLuint) const, &t_program::f_get_active_uniform>())
		(L"get_attached_shaders", t_member<t_scoped(t_program::*)() const, &t_program::f_get_attached_shaders>())
		(L"get_attrib_location", t_member<GLint(t_program::*)(const std::wstring&) const, &t_program::f_get_attrib_location>())
		(L"get_parameteri", t_member<GLint(t_program::*)(GLenum) const, &t_program::f_get_parameteri>())
		(L"get_info_log", t_member<std::wstring(t_program::*)() const, &t_program::f_get_info_log>())
		(L"get_uniformfv", t_member<t_scoped(t_program::*)(const t_uniform_location&) const, &t_program::f_get_uniformfv>())
		(L"get_uniformiv", t_member<t_scoped(t_program::*)(const t_uniform_location&) const, &t_program::f_get_uniformiv>())
		(L"get_uniform_location", t_member<t_uniform_location(t_program::*)(const std::wstring&) const, &t_program::f_get_uniform_location>())
		(L"link", t_member<void(t_program::*)(), &t_program::f_link>())
		(L"validate", t_member<void(t_program::*)(), &t_program::f_validate>())
	;
}

t_scoped t_type_of<xemmaix::gl::t_program>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*), xemmaix::gl::t_program::f_construct>::t_bind<xemmaix::gl::t_program>::f_do(this, a_stack, a_n);
}

}

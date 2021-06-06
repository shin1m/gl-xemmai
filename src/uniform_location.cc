#include "uniform_location.h"

namespace xemmai
{

void t_type_of<xemmaix::gl::t_uniform_location>::f_define(t_library* a_library)
{
	using namespace xemmaix::gl;
	t_define{a_library}
		(L"uniform1f"sv, t_member<void(t_uniform_location::*)(GLfloat), &t_uniform_location::f_uniform1f>())
		(L"uniform1fv"sv, t_member<void(t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform1fv>())
		(L"uniform1i"sv, t_member<void(t_uniform_location::*)(GLint), &t_uniform_location::f_uniform1i>())
		(L"uniform1iv"sv, t_member<void(t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform1iv>())
		(L"uniform2f"sv, t_member<void(t_uniform_location::*)(GLfloat, GLfloat), &t_uniform_location::f_uniform2f>())
		(L"uniform2fv"sv, t_member<void(t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform2fv>())
		(L"uniform2i"sv, t_member<void(t_uniform_location::*)(GLint, GLint), &t_uniform_location::f_uniform2i>())
		(L"uniform2iv"sv, t_member<void(t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform2iv>())
		(L"uniform3f"sv, t_member<void(t_uniform_location::*)(GLfloat, GLfloat, GLfloat), &t_uniform_location::f_uniform3f>())
		(L"uniform3fv"sv, t_member<void(t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform3fv>())
		(L"uniform3i"sv, t_member<void(t_uniform_location::*)(GLint, GLint, GLint), &t_uniform_location::f_uniform3i>())
		(L"uniform3iv"sv, t_member<void(t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform3iv>())
		(L"uniform4f"sv, t_member<void(t_uniform_location::*)(GLfloat, GLfloat, GLfloat, GLfloat), &t_uniform_location::f_uniform4f>())
		(L"uniform4fv"sv, t_member<void(t_uniform_location::*)(const t_bytes&a_values), &t_uniform_location::f_uniform4fv>())
		(L"uniform4i"sv, t_member<void(t_uniform_location::*)(GLint, GLint, GLint, GLint), &t_uniform_location::f_uniform4i>())
		(L"uniform4iv"sv, t_member<void(t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform4iv>())
		(L"matrix2fv"sv, t_member<void(t_uniform_location::*)(bool, const t_bytes&), &t_uniform_location::f_matrix2fv>())
		(L"matrix3fv"sv, t_member<void(t_uniform_location::*)(bool, const t_bytes&), &t_uniform_location::f_matrix3fv>())
		(L"matrix4fv"sv, t_member<void(t_uniform_location::*)(bool, const t_bytes&), &t_uniform_location::f_matrix4fv>())
	.f_derive<t_uniform_location, t_object>();
}

}

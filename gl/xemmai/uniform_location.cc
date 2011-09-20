#include "uniform_location.h"

namespace xemmai
{

void t_type_of<t_uniform_location>::f_define(t_extension* a_extension)
{
	t_define<t_uniform_location, t_object>(a_extension, L"UniformLocation")
		(L"uniform1f", t_member<void (t_uniform_location::*)(GLfloat), &t_uniform_location::f_uniform1f>())
		(L"uniform1fv", t_member<void (t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform1fv>())
		(L"uniform1i", t_member<void (t_uniform_location::*)(GLint), &t_uniform_location::f_uniform1i>())
		(L"uniform1iv", t_member<void (t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform1iv>())
		(L"uniform2f", t_member<void (t_uniform_location::*)(GLfloat, GLfloat), &t_uniform_location::f_uniform2f>())
		(L"uniform2fv", t_member<void (t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform2fv>())
		(L"uniform2i", t_member<void (t_uniform_location::*)(GLint, GLint), &t_uniform_location::f_uniform2i>())
		(L"uniform2iv", t_member<void (t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform2iv>())
		(L"uniform3f", t_member<void (t_uniform_location::*)(GLfloat, GLfloat, GLfloat), &t_uniform_location::f_uniform3f>())
		(L"uniform3fv", t_member<void (t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform3fv>())
		(L"uniform3i", t_member<void (t_uniform_location::*)(GLint, GLint, GLint), &t_uniform_location::f_uniform3i>())
		(L"uniform3iv", t_member<void (t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform3iv>())
		(L"uniform4f", t_member<void (t_uniform_location::*)(GLfloat, GLfloat, GLfloat, GLfloat), &t_uniform_location::f_uniform4f>())
		(L"uniform4fv", t_member<void (t_uniform_location::*)(const t_bytes&a_values), &t_uniform_location::f_uniform4fv>())
		(L"uniform4i", t_member<void (t_uniform_location::*)(GLint, GLint, GLint, GLint), &t_uniform_location::f_uniform4i>())
		(L"uniform4iv", t_member<void (t_uniform_location::*)(const t_bytes&), &t_uniform_location::f_uniform4iv>())
		(L"matrix2fv", t_member<void (t_uniform_location::*)(bool, const t_bytes&), &t_uniform_location::f_matrix2fv>())
		(L"matrix3fv", t_member<void (t_uniform_location::*)(bool, const t_bytes&), &t_uniform_location::f_matrix3fv>())
		(L"matrix4fv", t_member<void (t_uniform_location::*)(bool, const t_bytes&), &t_uniform_location::f_matrix4fv>())
	;
}

t_type* t_type_of<t_uniform_location>::f_derive(t_object* a_this)
{
	return 0;
}

void t_type_of<t_uniform_location>::f_finalize(t_object* a_this)
{
	delete &f_as<t_uniform_location&>(a_this);
}

void t_type_of<t_uniform_location>::f_instantiate(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}

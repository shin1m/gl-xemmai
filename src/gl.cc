#ifdef _WIN32
#include <cstdint>
#endif

#include "array.h"
#include "buffer.h"
#include "framebuffer.h"
#include "renderbuffer.h"
#include "texture.h"
#include "program.h"

namespace xemmaix::gl
{

using namespace xemmai;

std::atomic_flag t_session::v_running = ATOMIC_FLAG_INIT;
XEMMAI__PORTABLE__THREAD t_session* t_session::v_instance;
#ifdef _WIN32
bool t_session::v_glew = false;
#endif

t_session::t_session(t_library* a_library) : v_library(a_library)
{
	if (v_running.test_and_set(std::memory_order_acquire)) f_throw(L"main already running."sv);
	v_instance = this;
}

t_session::~t_session()
{
	while (!v_buffers.empty()) v_buffers.begin()->second->f_as<t_buffer>().f_delete();
	while (!v_framebuffers.empty()) v_framebuffers.begin()->second->f_as<t_framebuffer>().f_delete();
	while (!v_renderbuffers.empty()) v_renderbuffers.begin()->second->f_as<t_renderbuffer>().f_delete();
	while (!v_textures.empty()) v_textures.begin()->second->f_as<t_texture>().f_delete();
	while (!v_programs.empty()) v_programs.begin()->second->f_as<t_program>().f_delete();
	while (!v_shaders.empty()) v_shaders.begin()->second->f_as<t_shader>().f_delete();
	v_instance = nullptr;
	v_running.clear(std::memory_order_release);
}

namespace
{

inline void f_main(t_library* a_library, const t_pvalue& a_callable)
{
	t_session session(a_library);
	a_callable();
}

inline void f_active_texture(GLenum a_texture)
{
	glActiveTexture(a_texture);
	t_error::f_check();
}

inline void f_bind_buffer(GLenum a_target, const t_buffer* a_buffer)
{
	glBindBuffer(a_target, a_buffer ? a_buffer->f_id() : 0);
	t_error::f_check();
}

inline void f_bind_framebuffer(GLenum a_target, const t_framebuffer* a_framebuffer)
{
	glBindFramebuffer(a_target, a_framebuffer ? a_framebuffer->f_id() : 0);
	t_error::f_check();
}

inline void f_bind_renderbuffer(GLenum a_target, const t_renderbuffer* a_renderbuffer)
{
	glBindRenderbuffer(a_target, a_renderbuffer ? a_renderbuffer->f_id() : 0);
	t_error::f_check();
}

inline void f_bind_texture(GLenum a_target, const t_texture* a_texture)
{
	glBindTexture(a_target, a_texture ? a_texture->f_id() : 0);
	t_error::f_check();
}

inline void f_blend_color(GLclampf a_red, GLclampf a_green, GLclampf a_blue, GLclampf a_alpha)
{
	glBlendColor(a_red, a_green, a_blue, a_alpha);
	t_error::f_check();
}

inline void f_blend_equation(GLenum a_mode)
{
	glBlendEquation(a_mode);
	t_error::f_check();
}

inline void f_blend_equation_separate(GLenum a_mode_rgb, GLenum a_mode_alpha)
{
	glBlendEquationSeparate(a_mode_rgb, a_mode_alpha);
	t_error::f_check();
}

inline void f_blend_func(GLenum a_sfactor, GLenum a_dfactor)
{
	glBlendFunc(a_sfactor, a_dfactor);
	t_error::f_check();
}

inline void f_blend_func_separate(GLenum a_sfactor_rgb, GLenum a_dfactor_rgb, GLenum a_sfactor_alpha, GLenum a_dfactor_alpha)
{
	glBlendFuncSeparate(a_sfactor_rgb, a_dfactor_rgb, a_sfactor_alpha, a_dfactor_alpha);
	t_error::f_check();
}

inline void f_buffer_data(GLenum a_target, const t_bytes& a_data, GLenum a_usage)
{
	glBufferData(a_target, a_data.f_size(), &a_data[0], a_usage);
	t_error::f_check();
}

inline void f_buffer_sub_data(GLenum a_target, GLintptr a_offset, const t_bytes& a_data)
{
	glBufferSubData(a_target, a_offset, a_data.f_size(), &a_data[0]);
	t_error::f_check();
}

inline GLenum f_check_framebuffer_status(GLenum a_target)
{
	return glCheckFramebufferStatus(a_target);
}

inline void f_clear(GLbitfield a_mask)
{
	glClear(a_mask);
	t_error::f_check();
}

inline void f_clear_color(GLclampf a_red, GLclampf a_green, GLclampf a_blue, GLclampf a_alpha)
{
	glClearColor(a_red, a_green, a_blue, a_alpha);
	t_error::f_check();
}

inline void f_clear_depthf(GLclampf a_depth)
{
	glClearDepthf(a_depth);
	t_error::f_check();
}

inline void f_clear_stencil(GLint a_s)
{
	glClearStencil(a_s);
	t_error::f_check();
}

inline void f_color_mask(bool a_red, bool a_green, bool a_blue, bool a_alpha)
{
	glColorMask(a_red ? GL_TRUE : GL_FALSE, a_green ? GL_TRUE : GL_FALSE, a_blue ? GL_TRUE : GL_FALSE, a_alpha ? GL_TRUE : GL_FALSE);
	t_error::f_check();
}

inline void f_copy_tex_image2d(GLenum a_target, GLint a_level, GLenum a_internal_format, GLint a_x, GLint a_y, GLsizei a_width, GLsizei a_height, GLint a_border)
{
	glCopyTexImage2D(a_target, a_level, a_internal_format, a_x, a_y, a_width, a_height, a_border);
	t_error::f_check();
}

inline void f_copy_tex_sub_image2d(GLenum a_target, GLint a_level, GLint a_xoffset, GLint a_yoffset, GLint a_x, GLint a_y, GLsizei a_width, GLsizei a_height)
{
	glCopyTexSubImage2D(a_target, a_level, a_xoffset, a_yoffset, a_x, a_y, a_width, a_height);
	t_error::f_check();
}

inline void f_cull_face(GLenum a_mode)
{
	glCullFace(a_mode);
	t_error::f_check();
}

inline void f_depth_func(GLenum a_func)
{
	glDepthFunc(a_func);
	t_error::f_check();
}

inline void f_depth_mask(bool a_flag)
{
	glDepthMask(a_flag ? GL_TRUE : GL_FALSE);
	t_error::f_check();
}

inline void f_depth_rangef(GLclampf a_n, GLclampf a_f)
{
	glDepthRangef(a_n, a_f);
	t_error::f_check();
}

inline void f_disable(GLenum a_cap)
{
	glDisable(a_cap);
	t_error::f_check();
}

inline void f_disable_vertex_attrib_array(GLuint a_index)
{
	glDisableVertexAttribArray(a_index);
	t_error::f_check();
}

inline void f_draw_arrays(GLenum a_mode, GLint a_first, GLsizei a_count)
{
	glDrawArrays(a_mode, a_first, a_count);
	t_error::f_check();
}

inline void f_draw_elements(GLenum a_mode, GLsizei a_count, GLenum a_type, GLintptr a_offset)
{
	glDrawElements(a_mode, a_count, a_type, reinterpret_cast<const GLvoid*>(a_offset));
	t_error::f_check();
}

inline void f_enable(GLenum a_cap)
{
	glEnable(a_cap);
	t_error::f_check();
}

inline void f_enable_vertex_attrib_array(GLuint a_index)
{
	glEnableVertexAttribArray(a_index);
	t_error::f_check();
}

inline void f_finish()
{
	glFinish();
	t_error::f_check();
}

inline void f_flush()
{
	glFlush();
	t_error::f_check();
}

inline void f_framebuffer_renderbuffer(GLenum a_target, GLenum a_attachment, GLenum a_renderbuffertarget, const t_renderbuffer* a_renderbuffer)
{
	glFramebufferRenderbuffer(a_target, a_attachment, a_renderbuffertarget, a_renderbuffer ? a_renderbuffer->f_id() : 0);
	t_error::f_check();
}

inline void f_framebuffer_texture2d(GLenum a_target, GLenum a_attachment, GLenum a_textarget, const t_texture* a_texture, GLint a_level)
{
	glFramebufferTexture2D(a_target, a_attachment, a_textarget, a_texture ? a_texture->f_id() : 0, a_level);
	t_error::f_check();
}

inline void f_front_face(GLenum a_mode)
{
	glFrontFace(a_mode);
	t_error::f_check();
}

inline void f_generate_mipmap(GLenum a_target)
{
	glGenerateMipmap(a_target);
	t_error::f_check();
}

inline bool f_get_boolean(GLenum a_name)
{
	GLboolean values[4];
	glGetBooleanv(a_name, values);
	return values[0] != GL_FALSE;
}

inline t_pvalue f_get_booleans(GLenum a_name)
{
	GLboolean values[4];
	glGetBooleanv(a_name, values);
	return f_tuple(values[0] != GL_FALSE, values[1] != GL_FALSE, values[2] != GL_FALSE, values[3] != GL_FALSE);
}

inline GLint f_get_buffer_parameter(GLenum a_target, GLenum a_name)
{
	GLint value;
	glGetBufferParameteriv(a_target, a_name, &value);
	return value;
}

inline GLfloat f_get_float(GLenum a_name)
{
	GLfloat values[4];
	glGetFloatv(a_name, values);
	return values[0];
}

inline t_pvalue f_get_floats(GLenum a_name)
{
	GLfloat values[4];
	glGetFloatv(a_name, values);
	return f_tuple(values[0], values[1], values[2], values[3]);
}

inline GLenum f_get_error()
{
	return glGetError();
}

inline GLint f_get_framebuffer_attachment_parameter(GLenum a_target, GLenum a_attachment, GLenum a_name)
{
	GLint value;
	glGetFramebufferAttachmentParameteriv(a_target, a_attachment, a_name, &value);
	return value;
}

inline GLint f_get_integer(GLenum a_name)
{
	GLint values[4];
	glGetIntegerv(a_name, values);
	return values[0];
}

inline t_pvalue f_get_integers(GLenum a_name)
{
	GLint values[4];
	glGetIntegerv(a_name, values);
	return f_tuple(values[0], values[1], values[2], values[3]);
}

inline GLint f_get_renderbuffer_parameter(GLenum a_target, GLenum a_name)
{
	GLint value;
	glGetRenderbufferParameteriv(a_target, a_name, &value);
	return value;
}

inline t_pvalue f_get_shader_precision_format(GLenum a_shader, GLenum a_precision)
{
	GLint range[2];
	GLint precision;
	glGetShaderPrecisionFormat(a_shader, a_precision, range, &precision);
	return f_tuple(range[0], range[1], precision);
}

inline std::wstring f_get_string(GLenum a_name)
{
	return f_convert(reinterpret_cast<const char*>(glGetString(a_name)));
}

inline GLfloat f_get_tex_parameterf(GLenum a_target, GLenum a_name)
{
	GLfloat values[4];
	glGetTexParameterfv(a_target, a_name, values);
	return values[0];
}

inline t_pvalue f_get_tex_parameterfv(GLenum a_target, GLenum a_name)
{
	GLfloat values[4];
	glGetTexParameterfv(a_target, a_name, values);
	return f_tuple(values[0], values[1], values[2], values[3]);
}

inline GLint f_get_tex_parameteri(GLenum a_target, GLenum a_name)
{
	GLint values[4];
	glGetTexParameteriv(a_target, a_name, values);
	return values[0];
}

inline t_pvalue f_get_tex_parameteriv(GLenum a_target, GLenum a_name)
{
	GLint values[4];
	glGetTexParameteriv(a_target, a_name, values);
	return f_tuple(values[0], values[1], values[2], values[3]);
}

inline GLfloat f_get_vertex_attribf(GLenum a_target, GLenum a_name)
{
	GLfloat values[4];
	glGetVertexAttribfv(a_target, a_name, values);
	return values[0];
}

inline t_pvalue f_get_vertex_attribfv(GLenum a_target, GLenum a_name)
{
	GLfloat values[4];
	glGetVertexAttribfv(a_target, a_name, values);
	return f_tuple(values[0], values[1], values[2], values[3]);
}

inline GLint f_get_vertex_attribi(GLenum a_target, GLenum a_name)
{
	GLint values[4];
	glGetVertexAttribiv(a_target, a_name, values);
	return values[0];
}

inline t_pvalue f_get_vertex_attribiv(GLenum a_target, GLenum a_name)
{
	GLint values[4];
	glGetVertexAttribiv(a_target, a_name, values);
	return f_tuple(values[0], values[1], values[2], values[3]);
}

inline GLintptr f_get_vertex_attrib_pointer(GLuint a_index, GLenum a_name)
{
	GLvoid* pointer;
	glGetVertexAttribPointerv(a_index, a_name, &pointer);
	return reinterpret_cast<GLintptr>(pointer);
}

inline void f_hint(GLenum a_target, GLenum a_mode)
{
	glHint(a_target, a_mode);
	t_error::f_check();
}

inline bool f_is_enabled(GLenum a_capability)
{
	return glIsEnabled(a_capability) != GL_FALSE;
}

inline void f_line_width(GLfloat a_width)
{
	glLineWidth(a_width);
	t_error::f_check();
}

inline void f_pixel_storei(GLenum a_pname, GLint a_param)
{
	glPixelStorei(a_pname, a_param);
	t_error::f_check();
}

inline void f_polygon_offset(GLfloat a_factor, GLfloat a_units)
{
	glPolygonOffset(a_factor, a_units);
	t_error::f_check();
}

inline void f_read_pixels(GLint a_x, GLint a_y, GLsizei a_width, GLsizei a_height, GLenum a_format, GLenum a_type, t_bytes& a_data)
{
	size_t n;
	switch (a_type) {
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_FLOAT:
		switch (a_format) {
		case GL_DEPTH_COMPONENT:
		case GL_ALPHA:
			n = 1;
			break;
		case GL_RGB:
			n = 3;
			break;
		case GL_RGBA:
			n = 4;
			break;
		case GL_LUMINANCE:
			n = 1;
			break;
		case GL_LUMINANCE_ALPHA:
			n = 2;
			break;
		default:
			t_error::f_throw(GL_INVALID_ENUM);
		}
		switch (a_type) {
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			n *= sizeof(char);
			break;
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
			n *= sizeof(short);
			break;
		case GL_INT:
		case GL_UNSIGNED_INT:
			n *= sizeof(int);
			break;
		case GL_FLOAT:
			n *= sizeof(float);
			break;
		default:
			t_error::f_throw(GL_INVALID_ENUM);
		}
		break;
	case GL_UNSIGNED_SHORT_4_4_4_4:
	case GL_UNSIGNED_SHORT_5_5_5_1:
		if (a_format != GL_RGBA) t_error::f_throw(GL_INVALID_ENUM);
		n = sizeof(short);
		break;
	case GL_UNSIGNED_SHORT_5_6_5:
		if (a_format != GL_RGB) t_error::f_throw(GL_INVALID_ENUM);
		n = sizeof(short);
		break;
	default:
		t_error::f_throw(GL_INVALID_ENUM);
	}
	if (a_data.f_size() < a_width * a_height * n) t_error::f_throw(GL_INVALID_OPERATION);
	glReadPixels(a_x, a_y, a_width, a_height, a_format, a_type, &a_data[0]);
	t_error::f_check();
}

inline void f_release_shader_compiler()
{
	glReleaseShaderCompiler();
	t_error::f_check();
}

inline void f_renderbuffer_storage(GLenum a_target, GLenum a_internalformat, GLsizei a_width, GLsizei a_height)
{
	glRenderbufferStorage(a_target, a_internalformat, a_width, a_height);
	t_error::f_check();
}

inline void f_sample_coverage(GLclampf a_value, bool a_invert)
{
	glSampleCoverage(a_value, a_invert ? GL_TRUE : GL_FALSE);
	t_error::f_check();
}

inline void f_scissor(GLint a_x, GLint a_y, GLsizei a_width, GLsizei a_height)
{
	glScissor(a_x, a_y, a_width, a_height);
	t_error::f_check();
}

inline void f_stencil_func(GLenum a_func, GLint a_ref, GLuint a_mask)
{
	glStencilFunc(a_func, a_ref, a_mask);
	t_error::f_check();
}

inline void f_stencil_func_separate(GLenum a_frontfunc, GLenum a_backfunc, GLint a_ref, GLuint a_mask)
{
	glStencilFuncSeparate(a_frontfunc, a_backfunc, a_ref, a_mask);
	t_error::f_check();
}

inline void f_stencil_mask(GLuint a_mask)
{
	glStencilMask(a_mask);
	t_error::f_check();
}

inline void f_stencil_mask_separate(GLenum a_face, GLuint a_mask)
{
	glStencilMaskSeparate(a_face, a_mask);
	t_error::f_check();
}

inline void f_stencil_op(GLenum a_fail, GLenum a_zfail, GLenum a_zpass)
{
	glStencilOp(a_fail, a_zfail, a_zpass);
	t_error::f_check();
}

inline void f_stencil_op_separate(GLenum a_face, GLenum a_sfail, GLenum a_dpfail, GLenum a_dppass)
{
	glStencilOpSeparate(a_face, a_sfail, a_dpfail, a_dppass);
	t_error::f_check();
}

inline void f_tex_image2d(GLenum a_target, GLint a_level, GLint a_internalformat, GLsizei a_width, GLsizei a_height, GLint a_border, GLenum a_format, GLenum a_type, const t_bytes& a_pixels)
{
	glTexImage2D(a_target, a_level, a_internalformat, a_width, a_height, a_border, a_format, a_type, &a_pixels[0]);
	t_error::f_check();
}

inline void f_tex_parameterf(GLenum a_target, GLenum a_pname, GLfloat a_param)
{
	glTexParameterf(a_target, a_pname, a_param);
	t_error::f_check();
}

inline void f_tex_parameter4f(GLenum a_target, GLenum a_name, GLfloat a_value0, GLfloat a_value1, GLfloat a_value2, GLfloat a_value3)
{
	GLfloat values[] = {a_value0, a_value1, a_value2, a_value3};
	glTexParameterfv(a_target, a_name, values);
	t_error::f_check();
}

inline void f_tex_parameteri(GLenum a_target, GLenum a_pname, GLint a_param)
{
	glTexParameteri(a_target, a_pname, a_param);
	t_error::f_check();
}

inline void f_tex_parameter4i(GLenum a_target, GLenum a_name, GLint a_value0, GLint a_value1, GLint a_value2, GLint a_value3)
{
	GLint values[] = {a_value0, a_value1, a_value2, a_value3};
	glTexParameteriv(a_target, a_name, values);
	t_error::f_check();
}

inline void f_tex_sub_image2d(GLenum a_target, GLint a_level, GLint a_xoffset, GLint a_yoffset, GLsizei a_width, GLsizei a_height, GLenum a_format, GLenum a_type, const t_bytes& a_pixels)
{
	glTexSubImage2D(a_target, a_level, a_xoffset, a_yoffset, a_width, a_height, a_format, a_type, &a_pixels[0]);
	t_error::f_check();
}

inline void f_vertex_attrib_pointer(GLuint a_index, GLint a_size, GLenum a_type, bool a_normalized, GLsizei a_stride, GLintptr a_offset)
{
	glVertexAttribPointer(a_index, a_size, a_type, a_normalized ? GL_TRUE : GL_FALSE, a_stride, reinterpret_cast<const GLvoid*>(a_offset));
	t_error::f_check();
}

inline void f_viewport(GLint a_x, GLint a_y, GLsizei a_width, GLsizei a_height)
{
	glViewport(a_x, a_y, a_width, a_height);
	t_error::f_check();
}

inline void f_use_program(const t_program* a_program)
{
	glUseProgram(a_program ? a_program->f_id() : 0);
	t_error::f_check();
}

inline void f_vertex_attrib1f(GLuint a_index, GLfloat a_x)
{
	glVertexAttrib1f(a_index, a_x);
	t_error::f_check();
}

inline void f_vertex_attrib2f(GLuint a_index, GLfloat a_x, GLfloat a_y)
{
	glVertexAttrib2f(a_index, a_x, a_y);
	t_error::f_check();
}

inline void f_vertex_attrib3f(GLuint a_index, GLfloat a_x, GLfloat a_y, GLfloat a_z)
{
	glVertexAttrib3f(a_index, a_x, a_y, a_z);
	t_error::f_check();
}

inline void f_vertex_attrib4f(GLuint a_index, GLfloat a_x, GLfloat a_y, GLfloat a_z, GLfloat a_w)
{
	glVertexAttrib4f(a_index, a_x, a_y, a_z, a_w);
	t_error::f_check();
}

}

void t_library::f_scan(t_scan a_scan)
{
	a_scan(v_type_array_of_int16);
	a_scan(v_type_array_of_int32);
	a_scan(v_type_array_of_float32);
	a_scan(v_type_error);
	a_scan(v_type_buffer);
	a_scan(v_type_framebuffer);
	a_scan(v_type_renderbuffer);
	a_scan(v_type_texture);
	a_scan(v_type_program);
	a_scan(v_type_shader);
	a_scan(v_type_uniform_location);
}

std::vector<std::pair<t_root, t_rvalue>> t_library::f_define()
{
	t_type_of<t_array_of<int16_t>>::f_define(this);
	t_type_of<t_array_of<int32_t>>::f_define(this);
	t_type_of<t_array_of<float>>::f_define(this);
	t_type_of<t_error>::f_define(this);
	t_type_of<t_buffer>::f_define(this);
	t_type_of<t_framebuffer>::f_define(this);
	t_type_of<t_renderbuffer>::f_define(this);
	t_type_of<t_texture>::f_define(this);
	t_type_of<t_program>::f_define(this);
	t_type_of<t_shader>::f_define(this);
	t_type_of<t_uniform_location>::f_define(this);
	return t_define(this)
		(L"Int16Array"sv, t_object::f_of(v_type_array_of_int16))
		(L"Int32Array"sv, t_object::f_of(v_type_array_of_int32))
		(L"Float32Array"sv, t_object::f_of(v_type_array_of_float32))
		(L"Error"sv, t_object::f_of(v_type_error))
		(L"Buffer"sv, t_object::f_of(v_type_buffer))
		(L"Framebuffer"sv, t_object::f_of(v_type_framebuffer))
		(L"Renderbuffer"sv, t_object::f_of(v_type_renderbuffer))
		(L"Texture"sv, t_object::f_of(v_type_texture))
		(L"Program"sv, t_object::f_of(v_type_program))
		(L"Shader"sv, t_object::f_of(v_type_shader))
		(L"UniformLocation"sv, t_object::f_of(v_type_uniform_location))
		(L"main"sv, t_static<void(*)(t_library*, const t_pvalue&), f_main>())
		(L"active_texture"sv, t_static<void(*)(GLenum), f_active_texture>())
		(L"bind_buffer"sv, t_static<void(*)(GLenum, const t_buffer*), f_bind_buffer>())
		(L"bind_framebuffer"sv, t_static<void(*)(GLenum, const t_framebuffer*), f_bind_framebuffer>())
		(L"bind_renderbuffer"sv, t_static<void(*)(GLenum, const t_renderbuffer*), f_bind_renderbuffer>())
		(L"bind_texture"sv, t_static<void(*)(GLenum, const t_texture*), f_bind_texture>())
		(L"blend_color"sv, t_static<void(*)(GLclampf, GLclampf, GLclampf, GLclampf), f_blend_color>())
		(L"blend_equation"sv, t_static<void(*)(GLenum), f_blend_equation>())
		(L"blend_equation_separate"sv, t_static<void(*)(GLenum, GLenum), f_blend_equation_separate>())
		(L"blend_func"sv, t_static<void(*)(GLenum, GLenum), f_blend_func>())
		(L"blend_func_separate"sv, t_static<void(*)(GLenum, GLenum, GLenum, GLenum), f_blend_func_separate>())
		(L"buffer_data"sv, t_static<void(*)(GLenum, const t_bytes&, GLenum), f_buffer_data>())
		(L"buffer_sub_data"sv, t_static<void(*)(GLenum, GLintptr, const t_bytes&), f_buffer_sub_data>())
		(L"check_framebuffer_status"sv, t_static<GLenum(*)(GLenum), f_check_framebuffer_status>())
		(L"clear"sv, t_static<void(*)(GLbitfield), f_clear>())
		(L"clear_color"sv, t_static<void(*)(GLclampf, GLclampf, GLclampf, GLclampf), f_clear_color>())
		(L"clear_depthf"sv, t_static<void(*)(GLclampf), f_clear_depthf>())
		(L"clear_stencil"sv, t_static<void(*)(GLint), f_clear_stencil>())
		(L"color_mask"sv, t_static<void(*)(bool, bool, bool, bool), f_color_mask>())
		(L"copy_tex_image2d"sv, t_static<void(*)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint), f_copy_tex_image2d>())
		(L"copy_tex_sub_image2d"sv, t_static<void(*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei), f_copy_tex_sub_image2d>())
		(L"cull_face"sv, t_static<void(*)(GLenum), f_cull_face>())
		(L"depth_func"sv, t_static<void(*)(GLenum), f_depth_func>())
		(L"depth_mask"sv, t_static<void(*)(bool), f_depth_mask>())
		(L"depth_rangef"sv, t_static<void(*)(GLclampf, GLclampf), f_depth_rangef>())
		(L"disable"sv, t_static<void(*)(GLenum), f_disable>())
		(L"disable_vertex_attrib_array"sv, t_static<void(*)(GLuint), f_disable_vertex_attrib_array>())
		(L"draw_arrays"sv, t_static<void(*)(GLenum, GLint, GLsizei), f_draw_arrays>())
		(L"draw_elements"sv, t_static<void(*)(GLenum, GLsizei, GLenum, GLintptr), f_draw_elements>())
		(L"enable"sv, t_static<void(*)(GLenum), f_enable>())
		(L"enable_vertex_attrib_array"sv, t_static<void(*)(GLuint), f_enable_vertex_attrib_array>())
		(L"finish"sv, t_static<void(*)(), f_finish>())
		(L"flush"sv, t_static<void(*)(), f_flush>())
		(L"framebuffer_renderbuffer"sv, t_static<void(*)(GLenum, GLenum, GLenum, const t_renderbuffer*), f_framebuffer_renderbuffer>())
		(L"framebuffer_texture2d"sv, t_static<void(*)(GLenum, GLenum, GLenum, const t_texture*, GLint), f_framebuffer_texture2d>())
		(L"front_face"sv, t_static<void(*)(GLenum), f_front_face>())
		(L"generate_mipmap"sv, t_static<void(*)(GLenum), f_generate_mipmap>())
		(L"get_boolean"sv, t_static<bool(*)(GLenum), f_get_boolean>())
		(L"get_booleans"sv, t_static<t_pvalue(*)(GLenum), f_get_booleans>())
		(L"get_buffer_parameter"sv, t_static<GLint(*)(GLenum, GLenum), f_get_buffer_parameter>())
		(L"get_float"sv, t_static<GLfloat(*)(GLenum), f_get_float>())
		(L"get_floats"sv, t_static<t_pvalue(*)(GLenum), f_get_floats>())
		(L"get_error"sv, t_static<GLenum(*)(), f_get_error>())
		(L"get_framebuffer_attachment_parameter"sv, t_static<GLint(*)(GLenum, GLenum, GLenum), f_get_framebuffer_attachment_parameter>())
		(L"get_integer"sv, t_static<GLint(*)(GLenum), f_get_integer>())
		(L"get_integers"sv, t_static<t_pvalue(*)(GLenum), f_get_integers>())
		(L"get_renderbuffer_parameter"sv, t_static<GLint(*)(GLenum, GLenum), f_get_renderbuffer_parameter>())
		(L"get_shader_precision_format"sv, t_static<t_pvalue(*)(GLenum, GLenum), f_get_shader_precision_format>())
		(L"get_string"sv, t_static<std::wstring(*)(GLenum), f_get_string>())
		(L"get_tex_parameterf"sv, t_static<GLfloat(*)(GLenum, GLenum), f_get_tex_parameterf>())
		(L"get_tex_parameterfv"sv, t_static<t_pvalue(*)(GLenum, GLenum), f_get_tex_parameterfv>())
		(L"get_tex_parameteri"sv, t_static<GLint(*)(GLenum, GLenum), f_get_tex_parameteri>())
		(L"get_tex_parameteriv"sv, t_static<t_pvalue(*)(GLenum, GLenum), f_get_tex_parameteriv>())
		(L"get_vertex_attribf"sv, t_static<GLfloat(*)(GLuint, GLenum), f_get_vertex_attribf>())
		(L"get_vertex_attribfv"sv, t_static<t_pvalue(*)(GLuint, GLenum), f_get_vertex_attribfv>())
		(L"get_vertex_attribi"sv, t_static<GLint(*)(GLuint, GLenum), f_get_vertex_attribi>())
		(L"get_vertex_attribiv"sv, t_static<t_pvalue(*)(GLuint, GLenum), f_get_vertex_attribiv>())
		(L"get_vertex_attrib_pointer"sv, t_static<GLintptr(*)(GLuint, GLenum), f_get_vertex_attrib_pointer>())
		(L"hint"sv, t_static<void(*)(GLenum, GLenum), f_hint>())
		(L"is_enabled"sv, t_static<bool(*)(GLenum), f_is_enabled>())
		(L"line_width"sv, t_static<void(*)(GLfloat), f_line_width>())
		(L"pixel_storei"sv, t_static<void(*)(GLenum, GLint), f_pixel_storei>())
		(L"polygon_offset"sv, t_static<void(*)(GLfloat, GLfloat), f_polygon_offset>())
		(L"read_pixels"sv, t_static<void(*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, t_bytes&), f_read_pixels>())
		(L"release_shader_compiler"sv, t_static<void(*)(), f_release_shader_compiler>())
		(L"renderbuffer_storage"sv, t_static<void(*)(GLenum, GLenum, GLsizei, GLsizei), f_renderbuffer_storage>())
		(L"sample_coverage"sv, t_static<void(*)(GLclampf, bool), f_sample_coverage>())
		(L"scissor"sv, t_static<void(*)(GLint, GLint, GLsizei, GLsizei), f_scissor>())
		(L"stencil_func"sv, t_static<void(*)(GLenum, GLint, GLuint), f_stencil_func>())
		(L"stencil_func_separate"sv, t_static<void(*)(GLenum, GLenum, GLint, GLuint), f_stencil_func_separate>())
		(L"stencil_mask"sv, t_static<void(*)(GLuint), f_stencil_mask>())
		(L"stencil_mask_separate"sv, t_static<void(*)(GLenum, GLuint), f_stencil_mask_separate>())
		(L"stencil_op"sv, t_static<void(*)(GLenum, GLenum, GLenum), f_stencil_op>())
		(L"stencil_op_separate"sv, t_static<void(*)(GLenum, GLenum, GLenum, GLenum), f_stencil_op_separate>())
		(L"tex_image2d"sv, t_static<void(*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const t_bytes&), f_tex_image2d>())
		(L"tex_parameterf"sv, t_static<void(*)(GLenum, GLenum, GLfloat), f_tex_parameterf>())
		(L"tex_parameter4f"sv, t_static<void(*)(GLenum, GLenum, GLfloat, GLfloat, GLfloat, GLfloat), f_tex_parameter4f>())
		(L"tex_parameteri"sv, t_static<void(*)(GLenum, GLenum, GLint), f_tex_parameteri>())
		(L"tex_parameter4i"sv, t_static<void(*)(GLenum, GLenum, GLint, GLint, GLint, GLint), f_tex_parameter4i>())
		(L"tex_sub_image2d"sv, t_static<void(*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const t_bytes&), f_tex_sub_image2d>())
		(L"use_program"sv, t_static<void(*)(const t_program*), f_use_program>())
		(L"vertex_attrib1f"sv, t_static<void(*)(GLuint, GLfloat), f_vertex_attrib1f>())
		(L"vertex_attrib2f"sv, t_static<void(*)(GLuint, GLfloat, GLfloat), f_vertex_attrib2f>())
		(L"vertex_attrib3f"sv, t_static<void(*)(GLuint, GLfloat, GLfloat, GLfloat), f_vertex_attrib3f>())
		(L"vertex_attrib4f"sv, t_static<void(*)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat), f_vertex_attrib4f>())
		(L"vertex_attrib_pointer"sv, t_static<void(*)(GLuint, GLint, GLenum, bool, GLsizei, GLintptr), f_vertex_attrib_pointer>())
		(L"viewport"sv, t_static<void(*)(GLint, GLint, GLsizei, GLsizei), f_viewport>())
		(L"DEPTH_BUFFER_BIT"sv, GL_DEPTH_BUFFER_BIT)
		(L"STENCIL_BUFFER_BIT"sv, GL_STENCIL_BUFFER_BIT)
		(L"COLOR_BUFFER_BIT"sv, GL_COLOR_BUFFER_BIT)
		(L"FALSE"sv, GL_FALSE)
		(L"TRUE"sv, GL_TRUE)
		(L"POINTS"sv, GL_POINTS)
		(L"LINES"sv, GL_LINES)
		(L"LINE_LOOP"sv, GL_LINE_LOOP)
		(L"LINE_STRIP"sv, GL_LINE_STRIP)
		(L"TRIANGLES"sv, GL_TRIANGLES)
		(L"TRIANGLE_STRIP"sv, GL_TRIANGLE_STRIP)
		(L"TRIANGLE_FAN"sv, GL_TRIANGLE_FAN)
		(L"ZERO"sv, GL_ZERO)
		(L"ONE"sv, GL_ONE)
		(L"SRC_COLOR"sv, GL_SRC_COLOR)
		(L"ONE_MINUS_SRC_COLOR"sv, GL_ONE_MINUS_SRC_COLOR)
		(L"SRC_ALPHA"sv, GL_SRC_ALPHA)
		(L"ONE_MINUS_SRC_ALPHA"sv, GL_ONE_MINUS_SRC_ALPHA)
		(L"DST_ALPHA"sv, GL_DST_ALPHA)
		(L"ONE_MINUS_DST_ALPHA"sv, GL_ONE_MINUS_DST_ALPHA)
		(L"DST_COLOR"sv, GL_DST_COLOR)
		(L"ONE_MINUS_DST_COLOR"sv, GL_ONE_MINUS_DST_COLOR)
		(L"SRC_ALPHA_SATURATE"sv, GL_SRC_ALPHA_SATURATE)
		(L"FUNC_ADD"sv, GL_FUNC_ADD)
		(L"BLEND_EQUATION"sv, GL_BLEND_EQUATION)
		(L"BLEND_EQUATION_RGB"sv, GL_BLEND_EQUATION_RGB)
		(L"BLEND_EQUATION_ALPHA"sv, GL_BLEND_EQUATION_ALPHA)
		(L"FUNC_SUBTRACT"sv, GL_FUNC_SUBTRACT)
		(L"FUNC_REVERSE_SUBTRACT"sv, GL_FUNC_REVERSE_SUBTRACT)
		(L"BLEND_DST_RGB"sv, GL_BLEND_DST_RGB)
		(L"BLEND_SRC_RGB"sv, GL_BLEND_SRC_RGB)
		(L"BLEND_DST_ALPHA"sv, GL_BLEND_DST_ALPHA)
		(L"BLEND_SRC_ALPHA"sv, GL_BLEND_SRC_ALPHA)
		(L"CONSTANT_COLOR"sv, GL_CONSTANT_COLOR)
		(L"ONE_MINUS_CONSTANT_COLOR"sv, GL_ONE_MINUS_CONSTANT_COLOR)
		(L"CONSTANT_ALPHA"sv, GL_CONSTANT_ALPHA)
		(L"ONE_MINUS_CONSTANT_ALPHA"sv, GL_ONE_MINUS_CONSTANT_ALPHA)
		(L"BLEND_COLOR"sv, GL_BLEND_COLOR)
		(L"ARRAY_BUFFER"sv, GL_ARRAY_BUFFER)
		(L"ELEMENT_ARRAY_BUFFER"sv, GL_ELEMENT_ARRAY_BUFFER)
		(L"ARRAY_BUFFER_BINDING"sv, GL_ARRAY_BUFFER_BINDING)
		(L"ELEMENT_ARRAY_BUFFER_BINDING"sv, GL_ELEMENT_ARRAY_BUFFER_BINDING)
		(L"STREAM_DRAW"sv, GL_STREAM_DRAW)
		(L"STATIC_DRAW"sv, GL_STATIC_DRAW)
		(L"DYNAMIC_DRAW"sv, GL_DYNAMIC_DRAW)
		(L"BUFFER_SIZE"sv, GL_BUFFER_SIZE)
		(L"BUFFER_USAGE"sv, GL_BUFFER_USAGE)
		(L"CURRENT_VERTEX_ATTRIB"sv, GL_CURRENT_VERTEX_ATTRIB)
		(L"FRONT"sv, GL_FRONT)
		(L"BACK"sv, GL_BACK)
		(L"FRONT_AND_BACK"sv, GL_FRONT_AND_BACK)
		(L"TEXTURE_2D"sv, GL_TEXTURE_2D)
		(L"CULL_FACE"sv, GL_CULL_FACE)
		(L"BLEND"sv, GL_BLEND)
		(L"DITHER"sv, GL_DITHER)
		(L"STENCIL_TEST"sv, GL_STENCIL_TEST)
		(L"DEPTH_TEST"sv, GL_DEPTH_TEST)
		(L"SCISSOR_TEST"sv, GL_SCISSOR_TEST)
		(L"POLYGON_OFFSET_FILL"sv, GL_POLYGON_OFFSET_FILL)
		(L"SAMPLE_ALPHA_TO_COVERAGE"sv, GL_SAMPLE_ALPHA_TO_COVERAGE)
		(L"SAMPLE_COVERAGE"sv, GL_SAMPLE_COVERAGE)
		(L"NO_ERROR"sv, GL_NO_ERROR)
		(L"INVALID_ENUM"sv, GL_INVALID_ENUM)
		(L"INVALID_VALUE"sv, GL_INVALID_VALUE)
		(L"INVALID_OPERATION"sv, GL_INVALID_OPERATION)
		(L"OUT_OF_MEMORY"sv, GL_OUT_OF_MEMORY)
		(L"CW"sv, GL_CW)
		(L"CCW"sv, GL_CCW)
		(L"LINE_WIDTH"sv, GL_LINE_WIDTH)
		(L"ALIASED_POINT_SIZE_RANGE"sv, GL_ALIASED_POINT_SIZE_RANGE)
		(L"ALIASED_LINE_WIDTH_RANGE"sv, GL_ALIASED_LINE_WIDTH_RANGE)
		(L"CULL_FACE_MODE"sv, GL_CULL_FACE_MODE)
		(L"FRONT_FACE"sv, GL_FRONT_FACE)
		(L"DEPTH_RANGE"sv, GL_DEPTH_RANGE)
		(L"DEPTH_WRITEMASK"sv, GL_DEPTH_WRITEMASK)
		(L"DEPTH_CLEAR_VALUE"sv, GL_DEPTH_CLEAR_VALUE)
		(L"DEPTH_FUNC"sv, GL_DEPTH_FUNC)
		(L"STENCIL_CLEAR_VALUE"sv, GL_STENCIL_CLEAR_VALUE)
		(L"STENCIL_FUNC"sv, GL_STENCIL_FUNC)
		(L"STENCIL_FAIL"sv, GL_STENCIL_FAIL)
		(L"STENCIL_PASS_DEPTH_FAIL"sv, GL_STENCIL_PASS_DEPTH_FAIL)
		(L"STENCIL_PASS_DEPTH_PASS"sv, GL_STENCIL_PASS_DEPTH_PASS)
		(L"STENCIL_REF"sv, GL_STENCIL_REF)
		(L"STENCIL_VALUE_MASK"sv, GL_STENCIL_VALUE_MASK)
		(L"STENCIL_WRITEMASK"sv, GL_STENCIL_WRITEMASK)
		(L"STENCIL_BACK_FUNC"sv, GL_STENCIL_BACK_FUNC)
		(L"STENCIL_BACK_FAIL"sv, GL_STENCIL_BACK_FAIL)
		(L"STENCIL_BACK_PASS_DEPTH_FAIL"sv, GL_STENCIL_BACK_PASS_DEPTH_FAIL)
		(L"STENCIL_BACK_PASS_DEPTH_PASS"sv, GL_STENCIL_BACK_PASS_DEPTH_PASS)
		(L"STENCIL_BACK_REF"sv, GL_STENCIL_BACK_REF)
		(L"STENCIL_BACK_VALUE_MASK"sv, GL_STENCIL_BACK_VALUE_MASK)
		(L"STENCIL_BACK_WRITEMASK"sv, GL_STENCIL_BACK_WRITEMASK)
		(L"VIEWPORT"sv, GL_VIEWPORT)
		(L"SCISSOR_BOX"sv, GL_SCISSOR_BOX)
		(L"COLOR_CLEAR_VALUE"sv, GL_COLOR_CLEAR_VALUE)
		(L"COLOR_WRITEMASK"sv, GL_COLOR_WRITEMASK)
		(L"UNPACK_ALIGNMENT"sv, GL_UNPACK_ALIGNMENT)
		(L"PACK_ALIGNMENT"sv, GL_PACK_ALIGNMENT)
		(L"MAX_TEXTURE_SIZE"sv, GL_MAX_TEXTURE_SIZE)
		(L"MAX_VIEWPORT_DIMS"sv, GL_MAX_VIEWPORT_DIMS)
		(L"SUBPIXEL_BITS"sv, GL_SUBPIXEL_BITS)
		(L"RED_BITS"sv, GL_RED_BITS)
		(L"GREEN_BITS"sv, GL_GREEN_BITS)
		(L"BLUE_BITS"sv, GL_BLUE_BITS)
		(L"ALPHA_BITS"sv, GL_ALPHA_BITS)
		(L"DEPTH_BITS"sv, GL_DEPTH_BITS)
		(L"STENCIL_BITS"sv, GL_STENCIL_BITS)
		(L"POLYGON_OFFSET_UNITS"sv, GL_POLYGON_OFFSET_UNITS)
		(L"POLYGON_OFFSET_FACTOR"sv, GL_POLYGON_OFFSET_FACTOR)
		(L"TEXTURE_BINDING_2D"sv, GL_TEXTURE_BINDING_2D)
		(L"SAMPLE_BUFFERS"sv, GL_SAMPLE_BUFFERS)
		(L"SAMPLES"sv, GL_SAMPLES)
		(L"SAMPLE_COVERAGE_VALUE"sv, GL_SAMPLE_COVERAGE_VALUE)
		(L"SAMPLE_COVERAGE_INVERT"sv, GL_SAMPLE_COVERAGE_INVERT)
		(L"NUM_COMPRESSED_TEXTURE_FORMATS"sv, GL_NUM_COMPRESSED_TEXTURE_FORMATS)
		(L"COMPRESSED_TEXTURE_FORMATS"sv, GL_COMPRESSED_TEXTURE_FORMATS)
		(L"DONT_CARE"sv, GL_DONT_CARE)
		(L"FASTEST"sv, GL_FASTEST)
		(L"NICEST"sv, GL_NICEST)
		(L"GENERATE_MIPMAP_HINT"sv, GL_GENERATE_MIPMAP_HINT)
		(L"BYTE"sv, GL_BYTE)
		(L"UNSIGNED_BYTE"sv, GL_UNSIGNED_BYTE)
		(L"SHORT"sv, GL_SHORT)
		(L"UNSIGNED_SHORT"sv, GL_UNSIGNED_SHORT)
		(L"INT"sv, GL_INT)
		(L"UNSIGNED_INT"sv, GL_UNSIGNED_INT)
		(L"FLOAT"sv, GL_FLOAT)
		(L"FIXED"sv, GL_FIXED)
		(L"DEPTH_COMPONENT"sv, GL_DEPTH_COMPONENT)
		(L"ALPHA"sv, GL_ALPHA)
		(L"RGB"sv, GL_RGB)
		(L"RGBA"sv, GL_RGBA)
		(L"LUMINANCE"sv, GL_LUMINANCE)
		(L"LUMINANCE_ALPHA"sv, GL_LUMINANCE_ALPHA)
		(L"UNSIGNED_SHORT_4_4_4_4"sv, GL_UNSIGNED_SHORT_4_4_4_4)
		(L"UNSIGNED_SHORT_5_5_5_1"sv, GL_UNSIGNED_SHORT_5_5_5_1)
		(L"UNSIGNED_SHORT_5_6_5"sv, GL_UNSIGNED_SHORT_5_6_5)
		(L"FRAGMENT_SHADER"sv, GL_FRAGMENT_SHADER)
		(L"VERTEX_SHADER"sv, GL_VERTEX_SHADER)
		(L"MAX_VERTEX_ATTRIBS"sv, GL_MAX_VERTEX_ATTRIBS)
		(L"MAX_VERTEX_UNIFORM_VECTORS"sv, GL_MAX_VERTEX_UNIFORM_VECTORS)
		(L"MAX_VARYING_VECTORS"sv, GL_MAX_VARYING_VECTORS)
		(L"MAX_COMBINED_TEXTURE_IMAGE_UNITS"sv, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)
		(L"MAX_VERTEX_TEXTURE_IMAGE_UNITS"sv, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS)
		(L"MAX_TEXTURE_IMAGE_UNITS"sv, GL_MAX_TEXTURE_IMAGE_UNITS)
		(L"MAX_FRAGMENT_UNIFORM_VECTORS"sv, GL_MAX_FRAGMENT_UNIFORM_VECTORS)
		(L"SHADER_TYPE"sv, GL_SHADER_TYPE)
		(L"DELETE_STATUS"sv, GL_DELETE_STATUS)
		(L"LINK_STATUS"sv, GL_LINK_STATUS)
		(L"VALIDATE_STATUS"sv, GL_VALIDATE_STATUS)
		(L"ATTACHED_SHADERS"sv, GL_ATTACHED_SHADERS)
		(L"ACTIVE_UNIFORMS"sv, GL_ACTIVE_UNIFORMS)
		(L"ACTIVE_UNIFORM_MAX_LENGTH"sv, GL_ACTIVE_UNIFORM_MAX_LENGTH)
		(L"ACTIVE_ATTRIBUTES"sv, GL_ACTIVE_ATTRIBUTES)
		(L"ACTIVE_ATTRIBUTE_MAX_LENGTH"sv, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH)
		(L"SHADING_LANGUAGE_VERSION"sv, GL_SHADING_LANGUAGE_VERSION)
		(L"CURRENT_PROGRAM"sv, GL_CURRENT_PROGRAM)
		(L"NEVER"sv, GL_NEVER)
		(L"LESS"sv, GL_LESS)
		(L"EQUAL"sv, GL_EQUAL)
		(L"LEQUAL"sv, GL_LEQUAL)
		(L"GREATER"sv, GL_GREATER)
		(L"NOTEQUAL"sv, GL_NOTEQUAL)
		(L"GEQUAL"sv, GL_GEQUAL)
		(L"ALWAYS"sv, GL_ALWAYS)
		(L"KEEP"sv, GL_KEEP)
		(L"REPLACE"sv, GL_REPLACE)
		(L"INCR"sv, GL_INCR)
		(L"DECR"sv, GL_DECR)
		(L"INVERT"sv, GL_INVERT)
		(L"INCR_WRAP"sv, GL_INCR_WRAP)
		(L"DECR_WRAP"sv, GL_DECR_WRAP)
		(L"VENDOR"sv, GL_VENDOR)
		(L"RENDERER"sv, GL_RENDERER)
		(L"VERSION"sv, GL_VERSION)
		(L"EXTENSIONS"sv, GL_EXTENSIONS)
		(L"NEAREST"sv, GL_NEAREST)
		(L"LINEAR"sv, GL_LINEAR)
		(L"NEAREST_MIPMAP_NEAREST"sv, GL_NEAREST_MIPMAP_NEAREST)
		(L"LINEAR_MIPMAP_NEAREST"sv, GL_LINEAR_MIPMAP_NEAREST)
		(L"NEAREST_MIPMAP_LINEAR"sv, GL_NEAREST_MIPMAP_LINEAR)
		(L"LINEAR_MIPMAP_LINEAR"sv, GL_LINEAR_MIPMAP_LINEAR)
		(L"TEXTURE_MAG_FILTER"sv, GL_TEXTURE_MAG_FILTER)
		(L"TEXTURE_MIN_FILTER"sv, GL_TEXTURE_MIN_FILTER)
		(L"TEXTURE_WRAP_S"sv, GL_TEXTURE_WRAP_S)
		(L"TEXTURE_WRAP_T"sv, GL_TEXTURE_WRAP_T)
		(L"TEXTURE"sv, GL_TEXTURE)
		(L"TEXTURE_CUBE_MAP"sv, GL_TEXTURE_CUBE_MAP)
		(L"TEXTURE_BINDING_CUBE_MAP"sv, GL_TEXTURE_BINDING_CUBE_MAP)
		(L"TEXTURE_CUBE_MAP_POSITIVE_X"sv, GL_TEXTURE_CUBE_MAP_POSITIVE_X)
		(L"TEXTURE_CUBE_MAP_NEGATIVE_X"sv, GL_TEXTURE_CUBE_MAP_NEGATIVE_X)
		(L"TEXTURE_CUBE_MAP_POSITIVE_Y"sv, GL_TEXTURE_CUBE_MAP_POSITIVE_Y)
		(L"TEXTURE_CUBE_MAP_NEGATIVE_Y"sv, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y)
		(L"TEXTURE_CUBE_MAP_POSITIVE_Z"sv, GL_TEXTURE_CUBE_MAP_POSITIVE_Z)
		(L"TEXTURE_CUBE_MAP_NEGATIVE_Z"sv, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
		(L"MAX_CUBE_MAP_TEXTURE_SIZE"sv, GL_MAX_CUBE_MAP_TEXTURE_SIZE)
		(L"TEXTURE0"sv, GL_TEXTURE0)
		(L"TEXTURE1"sv, GL_TEXTURE1)
		(L"TEXTURE2"sv, GL_TEXTURE2)
		(L"TEXTURE3"sv, GL_TEXTURE3)
		(L"TEXTURE4"sv, GL_TEXTURE4)
		(L"TEXTURE5"sv, GL_TEXTURE5)
		(L"TEXTURE6"sv, GL_TEXTURE6)
		(L"TEXTURE7"sv, GL_TEXTURE7)
		(L"TEXTURE8"sv, GL_TEXTURE8)
		(L"TEXTURE9"sv, GL_TEXTURE9)
		(L"TEXTURE10"sv, GL_TEXTURE10)
		(L"TEXTURE11"sv, GL_TEXTURE11)
		(L"TEXTURE12"sv, GL_TEXTURE12)
		(L"TEXTURE13"sv, GL_TEXTURE13)
		(L"TEXTURE14"sv, GL_TEXTURE14)
		(L"TEXTURE15"sv, GL_TEXTURE15)
		(L"TEXTURE16"sv, GL_TEXTURE16)
		(L"TEXTURE17"sv, GL_TEXTURE17)
		(L"TEXTURE18"sv, GL_TEXTURE18)
		(L"TEXTURE19"sv, GL_TEXTURE19)
		(L"TEXTURE20"sv, GL_TEXTURE20)
		(L"TEXTURE21"sv, GL_TEXTURE21)
		(L"TEXTURE22"sv, GL_TEXTURE22)
		(L"TEXTURE23"sv, GL_TEXTURE23)
		(L"TEXTURE24"sv, GL_TEXTURE24)
		(L"TEXTURE25"sv, GL_TEXTURE25)
		(L"TEXTURE26"sv, GL_TEXTURE26)
		(L"TEXTURE27"sv, GL_TEXTURE27)
		(L"TEXTURE28"sv, GL_TEXTURE28)
		(L"TEXTURE29"sv, GL_TEXTURE29)
		(L"TEXTURE30"sv, GL_TEXTURE30)
		(L"TEXTURE31"sv, GL_TEXTURE31)
		(L"ACTIVE_TEXTURE"sv, GL_ACTIVE_TEXTURE)
		(L"REPEAT"sv, GL_REPEAT)
		(L"CLAMP_TO_EDGE"sv, GL_CLAMP_TO_EDGE)
		(L"MIRRORED_REPEAT"sv, GL_MIRRORED_REPEAT)
		(L"FLOAT_VEC2"sv, GL_FLOAT_VEC2)
		(L"FLOAT_VEC3"sv, GL_FLOAT_VEC3)
		(L"FLOAT_VEC4"sv, GL_FLOAT_VEC4)
		(L"INT_VEC2"sv, GL_INT_VEC2)
		(L"INT_VEC3"sv, GL_INT_VEC3)
		(L"INT_VEC4"sv, GL_INT_VEC4)
		(L"BOOL"sv, GL_BOOL)
		(L"BOOL_VEC2"sv, GL_BOOL_VEC2)
		(L"BOOL_VEC3"sv, GL_BOOL_VEC3)
		(L"BOOL_VEC4"sv, GL_BOOL_VEC4)
		(L"FLOAT_MAT2"sv, GL_FLOAT_MAT2)
		(L"FLOAT_MAT3"sv, GL_FLOAT_MAT3)
		(L"FLOAT_MAT4"sv, GL_FLOAT_MAT4)
		(L"SAMPLER_2D"sv, GL_SAMPLER_2D)
		(L"SAMPLER_CUBE"sv, GL_SAMPLER_CUBE)
		(L"VERTEX_ATTRIB_ARRAY_ENABLED"sv, GL_VERTEX_ATTRIB_ARRAY_ENABLED)
		(L"VERTEX_ATTRIB_ARRAY_SIZE"sv, GL_VERTEX_ATTRIB_ARRAY_SIZE)
		(L"VERTEX_ATTRIB_ARRAY_STRIDE"sv, GL_VERTEX_ATTRIB_ARRAY_STRIDE)
		(L"VERTEX_ATTRIB_ARRAY_TYPE"sv, GL_VERTEX_ATTRIB_ARRAY_TYPE)
		(L"VERTEX_ATTRIB_ARRAY_NORMALIZED"sv, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED)
		(L"VERTEX_ATTRIB_ARRAY_POINTER"sv, GL_VERTEX_ATTRIB_ARRAY_POINTER)
		(L"VERTEX_ATTRIB_ARRAY_BUFFER_BINDING"sv, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING)
		(L"IMPLEMENTATION_COLOR_READ_TYPE"sv, GL_IMPLEMENTATION_COLOR_READ_TYPE)
		(L"IMPLEMENTATION_COLOR_READ_FORMAT"sv, GL_IMPLEMENTATION_COLOR_READ_FORMAT)
		(L"COMPILE_STATUS"sv, GL_COMPILE_STATUS)
		(L"INFO_LOG_LENGTH"sv, GL_INFO_LOG_LENGTH)
		(L"SHADER_SOURCE_LENGTH"sv, GL_SHADER_SOURCE_LENGTH)
		(L"SHADER_COMPILER"sv, GL_SHADER_COMPILER)
//		(L"SHADER_BINARY_FORMATS"sv, GL_SHADER_BINARY_FORMATS)
		(L"NUM_SHADER_BINARY_FORMATS"sv, GL_NUM_SHADER_BINARY_FORMATS)
		(L"LOW_FLOAT"sv, GL_LOW_FLOAT)
		(L"MEDIUM_FLOAT"sv, GL_MEDIUM_FLOAT)
		(L"HIGH_FLOAT"sv, GL_HIGH_FLOAT)
		(L"LOW_INT"sv, GL_LOW_INT)
		(L"MEDIUM_INT"sv, GL_MEDIUM_INT)
		(L"HIGH_INT"sv, GL_HIGH_INT)
		(L"FRAMEBUFFER"sv, GL_FRAMEBUFFER)
		(L"RENDERBUFFER"sv, GL_RENDERBUFFER)
		(L"RGBA4"sv, GL_RGBA4)
		(L"RGB5_A1"sv, GL_RGB5_A1)
//		(L"RGB565"sv, GL_RGB565)
		(L"DEPTH_COMPONENT16"sv, GL_DEPTH_COMPONENT16)
		(L"STENCIL_INDEX"sv, GL_STENCIL_INDEX)
		(L"STENCIL_INDEX8"sv, GL_STENCIL_INDEX8)
		(L"RENDERBUFFER_WIDTH"sv, GL_RENDERBUFFER_WIDTH)
		(L"RENDERBUFFER_HEIGHT"sv, GL_RENDERBUFFER_HEIGHT)
		(L"RENDERBUFFER_INTERNAL_FORMAT"sv, GL_RENDERBUFFER_INTERNAL_FORMAT)
		(L"RENDERBUFFER_RED_SIZE"sv, GL_RENDERBUFFER_RED_SIZE)
		(L"RENDERBUFFER_GREEN_SIZE"sv, GL_RENDERBUFFER_GREEN_SIZE)
		(L"RENDERBUFFER_BLUE_SIZE"sv, GL_RENDERBUFFER_BLUE_SIZE)
		(L"RENDERBUFFER_ALPHA_SIZE"sv, GL_RENDERBUFFER_ALPHA_SIZE)
		(L"RENDERBUFFER_DEPTH_SIZE"sv, GL_RENDERBUFFER_DEPTH_SIZE)
		(L"RENDERBUFFER_STENCIL_SIZE"sv, GL_RENDERBUFFER_STENCIL_SIZE)
		(L"FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE"sv, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE)
		(L"FRAMEBUFFER_ATTACHMENT_OBJECT_NAME"sv, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME)
		(L"FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL"sv, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL)
		(L"FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE"sv, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE)
		(L"COLOR_ATTACHMENT0"sv, GL_COLOR_ATTACHMENT0)
		(L"DEPTH_ATTACHMENT"sv, GL_DEPTH_ATTACHMENT)
		(L"STENCIL_ATTACHMENT"sv, GL_STENCIL_ATTACHMENT)
		(L"NONE"sv, GL_NONE)
		(L"FRAMEBUFFER_COMPLETE"sv, GL_FRAMEBUFFER_COMPLETE)
		(L"FRAMEBUFFER_INCOMPLETE_ATTACHMENT"sv, GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
		(L"FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"sv, GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
//		(L"FRAMEBUFFER_INCOMPLETE_DIMENSIONS"sv, GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS)
		(L"FRAMEBUFFER_UNSUPPORTED"sv, GL_FRAMEBUFFER_UNSUPPORTED)
		(L"FRAMEBUFFER_BINDING"sv, GL_FRAMEBUFFER_BINDING)
		(L"RENDERBUFFER_BINDING"sv, GL_RENDERBUFFER_BINDING)
		(L"MAX_RENDERBUFFER_SIZE"sv, GL_MAX_RENDERBUFFER_SIZE)
		(L"INVALID_FRAMEBUFFER_OPERATION"sv, GL_INVALID_FRAMEBUFFER_OPERATION)
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmaix::gl::t_library>(a_handle);
}

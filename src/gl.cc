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

std::mutex t_session::v_mutex;
bool t_session::v_running = false;
XEMMAI__PORTABLE__THREAD t_session* t_session::v_instance;
#ifdef _WIN32
bool t_session::v_glew = false;
#endif

t_session::t_session(t_extension* a_extension) : v_extension(a_extension)
{
	std::unique_lock lock(v_mutex);
	if (v_running) f_throw(L"main already running."sv);
	v_running = true;
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
	std::unique_lock lock(v_mutex);
	v_running = false;
	v_instance = nullptr;
}

namespace
{

inline void f_main(t_extension* a_extension, const t_pvalue& a_callable)
{
	t_session session(a_extension);
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

t_extension::t_extension(t_object* a_module) : xemmai::t_extension(a_module)
{
	t_type_of<t_array_of<int16_t>>::f_define(this, L"Int16Array"sv);
	t_type_of<t_array_of<int32_t>>::f_define(this, L"Int32Array"sv);
	t_type_of<t_array_of<float>>::f_define(this, L"Float32Array"sv);
	t_type_of<t_error>::f_define(this);
	t_type_of<t_buffer>::f_define(this);
	t_type_of<t_framebuffer>::f_define(this);
	t_type_of<t_renderbuffer>::f_define(this);
	t_type_of<t_texture>::f_define(this);
	t_type_of<t_program>::f_define(this);
	t_type_of<t_shader>::f_define(this);
	t_type_of<t_uniform_location>::f_define(this);
	f_define<void(*)(t_extension*, const t_pvalue&), f_main>(this, L"main"sv);
	f_define<void(*)(GLenum), f_active_texture>(this, L"active_texture"sv);
	f_define<void(*)(GLenum, const t_buffer*), f_bind_buffer>(this, L"bind_buffer"sv);
	f_define<void(*)(GLenum, const t_framebuffer*), f_bind_framebuffer>(this, L"bind_framebuffer"sv);
	f_define<void(*)(GLenum, const t_renderbuffer*), f_bind_renderbuffer>(this, L"bind_renderbuffer"sv);
	f_define<void(*)(GLenum, const t_texture*), f_bind_texture>(this, L"bind_texture"sv);
	f_define<void(*)(GLclampf, GLclampf, GLclampf, GLclampf), f_blend_color>(this, L"blend_color"sv);
	f_define<void(*)(GLenum), f_blend_equation>(this, L"blend_equation"sv);
	f_define<void(*)(GLenum, GLenum), f_blend_equation_separate>(this, L"blend_equation_separate"sv);
	f_define<void(*)(GLenum, GLenum), f_blend_func>(this, L"blend_func"sv);
	f_define<void(*)(GLenum, GLenum, GLenum, GLenum), f_blend_func_separate>(this, L"blend_func_separate"sv);
	f_define<void(*)(GLenum, const t_bytes&, GLenum), f_buffer_data>(this, L"buffer_data"sv);
	f_define<void(*)(GLenum, GLintptr, const t_bytes&), f_buffer_sub_data>(this, L"buffer_sub_data"sv);
	f_define<GLenum(*)(GLenum), f_check_framebuffer_status>(this, L"check_framebuffer_status"sv);
	f_define<void(*)(GLbitfield), f_clear>(this, L"clear"sv);
	f_define<void(*)(GLclampf, GLclampf, GLclampf, GLclampf), f_clear_color>(this, L"clear_color"sv);
	f_define<void(*)(GLclampf), f_clear_depthf>(this, L"clear_depthf"sv);
	f_define<void(*)(GLint), f_clear_stencil>(this, L"clear_stencil"sv);
	f_define<void(*)(bool, bool, bool, bool), f_color_mask>(this, L"color_mask"sv);
	f_define<void(*)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint), f_copy_tex_image2d>(this, L"copy_tex_image2d"sv);
	f_define<void(*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei), f_copy_tex_sub_image2d>(this, L"copy_tex_sub_image2d"sv);
	f_define<void(*)(GLenum), f_cull_face>(this, L"cull_face"sv);
	f_define<void(*)(GLenum), f_depth_func>(this, L"depth_func"sv);
	f_define<void(*)(bool), f_depth_mask>(this, L"depth_mask"sv);
	f_define<void(*)(GLclampf, GLclampf), f_depth_rangef>(this, L"depth_rangef"sv);
	f_define<void(*)(GLenum), f_disable>(this, L"disable"sv);
	f_define<void(*)(GLuint), f_disable_vertex_attrib_array>(this, L"disable_vertex_attrib_array"sv);
	f_define<void(*)(GLenum, GLint, GLsizei), f_draw_arrays>(this, L"draw_arrays"sv);
	f_define<void(*)(GLenum, GLsizei, GLenum, GLintptr), f_draw_elements>(this, L"draw_elements"sv);
	f_define<void(*)(GLenum), f_enable>(this, L"enable"sv);
	f_define<void(*)(GLuint), f_enable_vertex_attrib_array>(this, L"enable_vertex_attrib_array"sv);
	f_define<void(*)(), f_finish>(this, L"finish"sv);
	f_define<void(*)(), f_flush>(this, L"flush"sv);
	f_define<void(*)(GLenum, GLenum, GLenum, const t_renderbuffer*), f_framebuffer_renderbuffer>(this, L"framebuffer_renderbuffer"sv);
	f_define<void(*)(GLenum, GLenum, GLenum, const t_texture*, GLint), f_framebuffer_texture2d>(this, L"framebuffer_texture2d"sv);
	f_define<void(*)(GLenum), f_front_face>(this, L"front_face"sv);
	f_define<void(*)(GLenum), f_generate_mipmap>(this, L"generate_mipmap"sv);
	f_define<bool(*)(GLenum), f_get_boolean>(this, L"get_boolean"sv);
	f_define<t_pvalue(*)(GLenum), f_get_booleans>(this, L"get_booleans"sv);
	f_define<GLint(*)(GLenum, GLenum), f_get_buffer_parameter>(this, L"get_buffer_parameter"sv);
	f_define<GLfloat(*)(GLenum), f_get_float>(this, L"get_float"sv);
	f_define<t_pvalue(*)(GLenum), f_get_floats>(this, L"get_floats"sv);
	f_define<GLenum(*)(), f_get_error>(this, L"get_error"sv);
	f_define<GLint(*)(GLenum, GLenum, GLenum), f_get_framebuffer_attachment_parameter>(this, L"get_framebuffer_attachment_parameter"sv);
	f_define<GLint(*)(GLenum), f_get_integer>(this, L"get_integer"sv);
	f_define<t_pvalue(*)(GLenum), f_get_integers>(this, L"get_integers"sv);
	f_define<GLint(*)(GLenum, GLenum), f_get_renderbuffer_parameter>(this, L"get_renderbuffer_parameter"sv);
	f_define<t_pvalue(*)(GLenum, GLenum), f_get_shader_precision_format>(this, L"get_shader_precision_format"sv);
	f_define<std::wstring(*)(GLenum), f_get_string>(this, L"get_string"sv);
	f_define<GLfloat(*)(GLenum, GLenum), f_get_tex_parameterf>(this, L"get_tex_parameterf"sv);
	f_define<t_pvalue(*)(GLenum, GLenum), f_get_tex_parameterfv>(this, L"get_tex_parameterfv"sv);
	f_define<GLint(*)(GLenum, GLenum), f_get_tex_parameteri>(this, L"get_tex_parameteri"sv);
	f_define<t_pvalue(*)(GLenum, GLenum), f_get_tex_parameteriv>(this, L"get_tex_parameteriv"sv);
	f_define<GLfloat(*)(GLuint, GLenum), f_get_vertex_attribf>(this, L"get_vertex_attribf"sv);
	f_define<t_pvalue(*)(GLuint, GLenum), f_get_vertex_attribfv>(this, L"get_vertex_attribfv"sv);
	f_define<GLint(*)(GLuint, GLenum), f_get_vertex_attribi>(this, L"get_vertex_attribi"sv);
	f_define<t_pvalue(*)(GLuint, GLenum), f_get_vertex_attribiv>(this, L"get_vertex_attribiv"sv);
	f_define<GLintptr(*)(GLuint, GLenum), f_get_vertex_attrib_pointer>(this, L"get_vertex_attrib_pointer"sv);
	f_define<void(*)(GLenum, GLenum), f_hint>(this, L"hint"sv);
	f_define<bool(*)(GLenum), f_is_enabled>(this, L"is_enabled"sv);
	f_define<void(*)(GLfloat), f_line_width>(this, L"line_width"sv);
	f_define<void(*)(GLenum, GLint), f_pixel_storei>(this, L"pixel_storei"sv);
	f_define<void(*)(GLfloat, GLfloat), f_polygon_offset>(this, L"polygon_offset"sv);
	f_define<void(*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, t_bytes&), f_read_pixels>(this, L"read_pixels"sv);
	f_define<void(*)(), f_release_shader_compiler>(this, L"release_shader_compiler"sv);
	f_define<void(*)(GLenum, GLenum, GLsizei, GLsizei), f_renderbuffer_storage>(this, L"renderbuffer_storage"sv);
	f_define<void(*)(GLclampf, bool), f_sample_coverage>(this, L"sample_coverage"sv);
	f_define<void(*)(GLint, GLint, GLsizei, GLsizei), f_scissor>(this, L"scissor"sv);
	f_define<void(*)(GLenum, GLint, GLuint), f_stencil_func>(this, L"stencil_func"sv);
	f_define<void(*)(GLenum, GLenum, GLint, GLuint), f_stencil_func_separate>(this, L"stencil_func_separate"sv);
	f_define<void(*)(GLuint), f_stencil_mask>(this, L"stencil_mask"sv);
	f_define<void(*)(GLenum, GLuint), f_stencil_mask_separate>(this, L"stencil_mask_separate"sv);
	f_define<void(*)(GLenum, GLenum, GLenum), f_stencil_op>(this, L"stencil_op"sv);
	f_define<void(*)(GLenum, GLenum, GLenum, GLenum), f_stencil_op_separate>(this, L"stencil_op_separate"sv);
	f_define<void(*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const t_bytes&), f_tex_image2d>(this, L"tex_image2d"sv);
	f_define<void(*)(GLenum, GLenum, GLfloat), f_tex_parameterf>(this, L"tex_parameterf"sv);
	f_define<void(*)(GLenum, GLenum, GLfloat, GLfloat, GLfloat, GLfloat), f_tex_parameter4f>(this, L"tex_parameter4f"sv);
	f_define<void(*)(GLenum, GLenum, GLint), f_tex_parameteri>(this, L"tex_parameteri"sv);
	f_define<void(*)(GLenum, GLenum, GLint, GLint, GLint, GLint), f_tex_parameter4i>(this, L"tex_parameter4i"sv);
	f_define<void(*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const t_bytes&), f_tex_sub_image2d>(this, L"tex_sub_image2d"sv);
	f_define<void(*)(const t_program*), f_use_program>(this, L"use_program"sv);
	f_define<void(*)(GLuint, GLfloat), f_vertex_attrib1f>(this, L"vertex_attrib1f"sv);
	f_define<void(*)(GLuint, GLfloat, GLfloat), f_vertex_attrib2f>(this, L"vertex_attrib2f"sv);
	f_define<void(*)(GLuint, GLfloat, GLfloat, GLfloat), f_vertex_attrib3f>(this, L"vertex_attrib3f"sv);
	f_define<void(*)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat), f_vertex_attrib4f>(this, L"vertex_attrib4f"sv);
	f_define<void(*)(GLuint, GLint, GLenum, bool, GLsizei, GLintptr), f_vertex_attrib_pointer>(this, L"vertex_attrib_pointer"sv);
	f_define<void(*)(GLint, GLint, GLsizei, GLsizei), f_viewport>(this, L"viewport"sv);
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_BUFFER_BIT"sv), f_as(GL_DEPTH_BUFFER_BIT));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BUFFER_BIT"sv), f_as(GL_STENCIL_BUFFER_BIT));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_BUFFER_BIT"sv), f_as(GL_COLOR_BUFFER_BIT));
	a_module->f_put(t_symbol::f_instantiate(L"FALSE"sv), f_as(GL_FALSE));
	a_module->f_put(t_symbol::f_instantiate(L"TRUE"sv), f_as(GL_TRUE));
	a_module->f_put(t_symbol::f_instantiate(L"POINTS"sv), f_as(GL_POINTS));
	a_module->f_put(t_symbol::f_instantiate(L"LINES"sv), f_as(GL_LINES));
	a_module->f_put(t_symbol::f_instantiate(L"LINE_LOOP"sv), f_as(GL_LINE_LOOP));
	a_module->f_put(t_symbol::f_instantiate(L"LINE_STRIP"sv), f_as(GL_LINE_STRIP));
	a_module->f_put(t_symbol::f_instantiate(L"TRIANGLES"sv), f_as(GL_TRIANGLES));
	a_module->f_put(t_symbol::f_instantiate(L"TRIANGLE_STRIP"sv), f_as(GL_TRIANGLE_STRIP));
	a_module->f_put(t_symbol::f_instantiate(L"TRIANGLE_FAN"sv), f_as(GL_TRIANGLE_FAN));
	a_module->f_put(t_symbol::f_instantiate(L"ZERO"sv), f_as(GL_ZERO));
	a_module->f_put(t_symbol::f_instantiate(L"ONE"sv), f_as(GL_ONE));
	a_module->f_put(t_symbol::f_instantiate(L"SRC_COLOR"sv), f_as(GL_SRC_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_SRC_COLOR"sv), f_as(GL_ONE_MINUS_SRC_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"SRC_ALPHA"sv), f_as(GL_SRC_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_SRC_ALPHA"sv), f_as(GL_ONE_MINUS_SRC_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"DST_ALPHA"sv), f_as(GL_DST_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_DST_ALPHA"sv), f_as(GL_ONE_MINUS_DST_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"DST_COLOR"sv), f_as(GL_DST_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_DST_COLOR"sv), f_as(GL_ONE_MINUS_DST_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"SRC_ALPHA_SATURATE"sv), f_as(GL_SRC_ALPHA_SATURATE));
	a_module->f_put(t_symbol::f_instantiate(L"FUNC_ADD"sv), f_as(GL_FUNC_ADD));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_EQUATION"sv), f_as(GL_BLEND_EQUATION));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_EQUATION_RGB"sv), f_as(GL_BLEND_EQUATION_RGB));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_EQUATION_ALPHA"sv), f_as(GL_BLEND_EQUATION_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"FUNC_SUBTRACT"sv), f_as(GL_FUNC_SUBTRACT));
	a_module->f_put(t_symbol::f_instantiate(L"FUNC_REVERSE_SUBTRACT"sv), f_as(GL_FUNC_REVERSE_SUBTRACT));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_DST_RGB"sv), f_as(GL_BLEND_DST_RGB));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_SRC_RGB"sv), f_as(GL_BLEND_SRC_RGB));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_DST_ALPHA"sv), f_as(GL_BLEND_DST_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_SRC_ALPHA"sv), f_as(GL_BLEND_SRC_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"CONSTANT_COLOR"sv), f_as(GL_CONSTANT_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_CONSTANT_COLOR"sv), f_as(GL_ONE_MINUS_CONSTANT_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"CONSTANT_ALPHA"sv), f_as(GL_CONSTANT_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_CONSTANT_ALPHA"sv), f_as(GL_ONE_MINUS_CONSTANT_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_COLOR"sv), f_as(GL_BLEND_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"ARRAY_BUFFER"sv), f_as(GL_ARRAY_BUFFER));
	a_module->f_put(t_symbol::f_instantiate(L"ELEMENT_ARRAY_BUFFER"sv), f_as(GL_ELEMENT_ARRAY_BUFFER));
	a_module->f_put(t_symbol::f_instantiate(L"ARRAY_BUFFER_BINDING"sv), f_as(GL_ARRAY_BUFFER_BINDING));
	a_module->f_put(t_symbol::f_instantiate(L"ELEMENT_ARRAY_BUFFER_BINDING"sv), f_as(GL_ELEMENT_ARRAY_BUFFER_BINDING));
	a_module->f_put(t_symbol::f_instantiate(L"STREAM_DRAW"sv), f_as(GL_STREAM_DRAW));
	a_module->f_put(t_symbol::f_instantiate(L"STATIC_DRAW"sv), f_as(GL_STATIC_DRAW));
	a_module->f_put(t_symbol::f_instantiate(L"DYNAMIC_DRAW"sv), f_as(GL_DYNAMIC_DRAW));
	a_module->f_put(t_symbol::f_instantiate(L"BUFFER_SIZE"sv), f_as(GL_BUFFER_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"BUFFER_USAGE"sv), f_as(GL_BUFFER_USAGE));
	a_module->f_put(t_symbol::f_instantiate(L"CURRENT_VERTEX_ATTRIB"sv), f_as(GL_CURRENT_VERTEX_ATTRIB));
	a_module->f_put(t_symbol::f_instantiate(L"FRONT"sv), f_as(GL_FRONT));
	a_module->f_put(t_symbol::f_instantiate(L"BACK"sv), f_as(GL_BACK));
	a_module->f_put(t_symbol::f_instantiate(L"FRONT_AND_BACK"sv), f_as(GL_FRONT_AND_BACK));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_2D"sv), f_as(GL_TEXTURE_2D));
	a_module->f_put(t_symbol::f_instantiate(L"CULL_FACE"sv), f_as(GL_CULL_FACE));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND"sv), f_as(GL_BLEND));
	a_module->f_put(t_symbol::f_instantiate(L"DITHER"sv), f_as(GL_DITHER));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_TEST"sv), f_as(GL_STENCIL_TEST));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_TEST"sv), f_as(GL_DEPTH_TEST));
	a_module->f_put(t_symbol::f_instantiate(L"SCISSOR_TEST"sv), f_as(GL_SCISSOR_TEST));
	a_module->f_put(t_symbol::f_instantiate(L"POLYGON_OFFSET_FILL"sv), f_as(GL_POLYGON_OFFSET_FILL));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLE_ALPHA_TO_COVERAGE"sv), f_as(GL_SAMPLE_ALPHA_TO_COVERAGE));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLE_COVERAGE"sv), f_as(GL_SAMPLE_COVERAGE));
	a_module->f_put(t_symbol::f_instantiate(L"NO_ERROR"sv), f_as(GL_NO_ERROR));
	a_module->f_put(t_symbol::f_instantiate(L"INVALID_ENUM"sv), f_as(GL_INVALID_ENUM));
	a_module->f_put(t_symbol::f_instantiate(L"INVALID_VALUE"sv), f_as(GL_INVALID_VALUE));
	a_module->f_put(t_symbol::f_instantiate(L"INVALID_OPERATION"sv), f_as(GL_INVALID_OPERATION));
	a_module->f_put(t_symbol::f_instantiate(L"OUT_OF_MEMORY"sv), f_as(GL_OUT_OF_MEMORY));
	a_module->f_put(t_symbol::f_instantiate(L"CW"sv), f_as(GL_CW));
	a_module->f_put(t_symbol::f_instantiate(L"CCW"sv), f_as(GL_CCW));
	a_module->f_put(t_symbol::f_instantiate(L"LINE_WIDTH"sv), f_as(GL_LINE_WIDTH));
	a_module->f_put(t_symbol::f_instantiate(L"ALIASED_POINT_SIZE_RANGE"sv), f_as(GL_ALIASED_POINT_SIZE_RANGE));
	a_module->f_put(t_symbol::f_instantiate(L"ALIASED_LINE_WIDTH_RANGE"sv), f_as(GL_ALIASED_LINE_WIDTH_RANGE));
	a_module->f_put(t_symbol::f_instantiate(L"CULL_FACE_MODE"sv), f_as(GL_CULL_FACE_MODE));
	a_module->f_put(t_symbol::f_instantiate(L"FRONT_FACE"sv), f_as(GL_FRONT_FACE));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_RANGE"sv), f_as(GL_DEPTH_RANGE));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_WRITEMASK"sv), f_as(GL_DEPTH_WRITEMASK));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_CLEAR_VALUE"sv), f_as(GL_DEPTH_CLEAR_VALUE));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_FUNC"sv), f_as(GL_DEPTH_FUNC));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_CLEAR_VALUE"sv), f_as(GL_STENCIL_CLEAR_VALUE));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_FUNC"sv), f_as(GL_STENCIL_FUNC));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_FAIL"sv), f_as(GL_STENCIL_FAIL));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_PASS_DEPTH_FAIL"sv), f_as(GL_STENCIL_PASS_DEPTH_FAIL));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_PASS_DEPTH_PASS"sv), f_as(GL_STENCIL_PASS_DEPTH_PASS));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_REF"sv), f_as(GL_STENCIL_REF));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_VALUE_MASK"sv), f_as(GL_STENCIL_VALUE_MASK));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_WRITEMASK"sv), f_as(GL_STENCIL_WRITEMASK));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_FUNC"sv), f_as(GL_STENCIL_BACK_FUNC));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_FAIL"sv), f_as(GL_STENCIL_BACK_FAIL));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_PASS_DEPTH_FAIL"sv), f_as(GL_STENCIL_BACK_PASS_DEPTH_FAIL));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_PASS_DEPTH_PASS"sv), f_as(GL_STENCIL_BACK_PASS_DEPTH_PASS));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_REF"sv), f_as(GL_STENCIL_BACK_REF));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_VALUE_MASK"sv), f_as(GL_STENCIL_BACK_VALUE_MASK));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_WRITEMASK"sv), f_as(GL_STENCIL_BACK_WRITEMASK));
	a_module->f_put(t_symbol::f_instantiate(L"VIEWPORT"sv), f_as(GL_VIEWPORT));
	a_module->f_put(t_symbol::f_instantiate(L"SCISSOR_BOX"sv), f_as(GL_SCISSOR_BOX));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_CLEAR_VALUE"sv), f_as(GL_COLOR_CLEAR_VALUE));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_WRITEMASK"sv), f_as(GL_COLOR_WRITEMASK));
	a_module->f_put(t_symbol::f_instantiate(L"UNPACK_ALIGNMENT"sv), f_as(GL_UNPACK_ALIGNMENT));
	a_module->f_put(t_symbol::f_instantiate(L"PACK_ALIGNMENT"sv), f_as(GL_PACK_ALIGNMENT));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_TEXTURE_SIZE"sv), f_as(GL_MAX_TEXTURE_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_VIEWPORT_DIMS"sv), f_as(GL_MAX_VIEWPORT_DIMS));
	a_module->f_put(t_symbol::f_instantiate(L"SUBPIXEL_BITS"sv), f_as(GL_SUBPIXEL_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"RED_BITS"sv), f_as(GL_RED_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"GREEN_BITS"sv), f_as(GL_GREEN_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"BLUE_BITS"sv), f_as(GL_BLUE_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"ALPHA_BITS"sv), f_as(GL_ALPHA_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_BITS"sv), f_as(GL_DEPTH_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BITS"sv), f_as(GL_STENCIL_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"POLYGON_OFFSET_UNITS"sv), f_as(GL_POLYGON_OFFSET_UNITS));
	a_module->f_put(t_symbol::f_instantiate(L"POLYGON_OFFSET_FACTOR"sv), f_as(GL_POLYGON_OFFSET_FACTOR));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_BINDING_2D"sv), f_as(GL_TEXTURE_BINDING_2D));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLE_BUFFERS"sv), f_as(GL_SAMPLE_BUFFERS));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLES"sv), f_as(GL_SAMPLES));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLE_COVERAGE_VALUE"sv), f_as(GL_SAMPLE_COVERAGE_VALUE));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLE_COVERAGE_INVERT"sv), f_as(GL_SAMPLE_COVERAGE_INVERT));
	a_module->f_put(t_symbol::f_instantiate(L"NUM_COMPRESSED_TEXTURE_FORMATS"sv), f_as(GL_NUM_COMPRESSED_TEXTURE_FORMATS));
	a_module->f_put(t_symbol::f_instantiate(L"COMPRESSED_TEXTURE_FORMATS"sv), f_as(GL_COMPRESSED_TEXTURE_FORMATS));
	a_module->f_put(t_symbol::f_instantiate(L"DONT_CARE"sv), f_as(GL_DONT_CARE));
	a_module->f_put(t_symbol::f_instantiate(L"FASTEST"sv), f_as(GL_FASTEST));
	a_module->f_put(t_symbol::f_instantiate(L"NICEST"sv), f_as(GL_NICEST));
	a_module->f_put(t_symbol::f_instantiate(L"GENERATE_MIPMAP_HINT"sv), f_as(GL_GENERATE_MIPMAP_HINT));
	a_module->f_put(t_symbol::f_instantiate(L"BYTE"sv), f_as(GL_BYTE));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_BYTE"sv), f_as(GL_UNSIGNED_BYTE));
	a_module->f_put(t_symbol::f_instantiate(L"SHORT"sv), f_as(GL_SHORT));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_SHORT"sv), f_as(GL_UNSIGNED_SHORT));
	a_module->f_put(t_symbol::f_instantiate(L"INT"sv), f_as(GL_INT));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_INT"sv), f_as(GL_UNSIGNED_INT));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT"sv), f_as(GL_FLOAT));
	a_module->f_put(t_symbol::f_instantiate(L"FIXED"sv), f_as(GL_FIXED));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_COMPONENT"sv), f_as(GL_DEPTH_COMPONENT));
	a_module->f_put(t_symbol::f_instantiate(L"ALPHA"sv), f_as(GL_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"RGB"sv), f_as(GL_RGB));
	a_module->f_put(t_symbol::f_instantiate(L"RGBA"sv), f_as(GL_RGBA));
	a_module->f_put(t_symbol::f_instantiate(L"LUMINANCE"sv), f_as(GL_LUMINANCE));
	a_module->f_put(t_symbol::f_instantiate(L"LUMINANCE_ALPHA"sv), f_as(GL_LUMINANCE_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_SHORT_4_4_4_4"sv), f_as(GL_UNSIGNED_SHORT_4_4_4_4));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_SHORT_5_5_5_1"sv), f_as(GL_UNSIGNED_SHORT_5_5_5_1));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_SHORT_5_6_5"sv), f_as(GL_UNSIGNED_SHORT_5_6_5));
	a_module->f_put(t_symbol::f_instantiate(L"FRAGMENT_SHADER"sv), f_as(GL_FRAGMENT_SHADER));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_SHADER"sv), f_as(GL_VERTEX_SHADER));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_VERTEX_ATTRIBS"sv), f_as(GL_MAX_VERTEX_ATTRIBS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_VERTEX_UNIFORM_VECTORS"sv), f_as(GL_MAX_VERTEX_UNIFORM_VECTORS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_VARYING_VECTORS"sv), f_as(GL_MAX_VARYING_VECTORS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_COMBINED_TEXTURE_IMAGE_UNITS"sv), f_as(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_VERTEX_TEXTURE_IMAGE_UNITS"sv), f_as(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_TEXTURE_IMAGE_UNITS"sv), f_as(GL_MAX_TEXTURE_IMAGE_UNITS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_FRAGMENT_UNIFORM_VECTORS"sv), f_as(GL_MAX_FRAGMENT_UNIFORM_VECTORS));
	a_module->f_put(t_symbol::f_instantiate(L"SHADER_TYPE"sv), f_as(GL_SHADER_TYPE));
	a_module->f_put(t_symbol::f_instantiate(L"DELETE_STATUS"sv), f_as(GL_DELETE_STATUS));
	a_module->f_put(t_symbol::f_instantiate(L"LINK_STATUS"sv), f_as(GL_LINK_STATUS));
	a_module->f_put(t_symbol::f_instantiate(L"VALIDATE_STATUS"sv), f_as(GL_VALIDATE_STATUS));
	a_module->f_put(t_symbol::f_instantiate(L"ATTACHED_SHADERS"sv), f_as(GL_ATTACHED_SHADERS));
	a_module->f_put(t_symbol::f_instantiate(L"ACTIVE_UNIFORMS"sv), f_as(GL_ACTIVE_UNIFORMS));
	a_module->f_put(t_symbol::f_instantiate(L"ACTIVE_UNIFORM_MAX_LENGTH"sv), f_as(GL_ACTIVE_UNIFORM_MAX_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"ACTIVE_ATTRIBUTES"sv), f_as(GL_ACTIVE_ATTRIBUTES));
	a_module->f_put(t_symbol::f_instantiate(L"ACTIVE_ATTRIBUTE_MAX_LENGTH"sv), f_as(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"SHADING_LANGUAGE_VERSION"sv), f_as(GL_SHADING_LANGUAGE_VERSION));
	a_module->f_put(t_symbol::f_instantiate(L"CURRENT_PROGRAM"sv), f_as(GL_CURRENT_PROGRAM));
	a_module->f_put(t_symbol::f_instantiate(L"NEVER"sv), f_as(GL_NEVER));
	a_module->f_put(t_symbol::f_instantiate(L"LESS"sv), f_as(GL_LESS));
	a_module->f_put(t_symbol::f_instantiate(L"EQUAL"sv), f_as(GL_EQUAL));
	a_module->f_put(t_symbol::f_instantiate(L"LEQUAL"sv), f_as(GL_LEQUAL));
	a_module->f_put(t_symbol::f_instantiate(L"GREATER"sv), f_as(GL_GREATER));
	a_module->f_put(t_symbol::f_instantiate(L"NOTEQUAL"sv), f_as(GL_NOTEQUAL));
	a_module->f_put(t_symbol::f_instantiate(L"GEQUAL"sv), f_as(GL_GEQUAL));
	a_module->f_put(t_symbol::f_instantiate(L"ALWAYS"sv), f_as(GL_ALWAYS));
	a_module->f_put(t_symbol::f_instantiate(L"KEEP"sv), f_as(GL_KEEP));
	a_module->f_put(t_symbol::f_instantiate(L"REPLACE"sv), f_as(GL_REPLACE));
	a_module->f_put(t_symbol::f_instantiate(L"INCR"sv), f_as(GL_INCR));
	a_module->f_put(t_symbol::f_instantiate(L"DECR"sv), f_as(GL_DECR));
	a_module->f_put(t_symbol::f_instantiate(L"INVERT"sv), f_as(GL_INVERT));
	a_module->f_put(t_symbol::f_instantiate(L"INCR_WRAP"sv), f_as(GL_INCR_WRAP));
	a_module->f_put(t_symbol::f_instantiate(L"DECR_WRAP"sv), f_as(GL_DECR_WRAP));
	a_module->f_put(t_symbol::f_instantiate(L"VENDOR"sv), f_as(GL_VENDOR));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERER"sv), f_as(GL_RENDERER));
	a_module->f_put(t_symbol::f_instantiate(L"VERSION"sv), f_as(GL_VERSION));
	a_module->f_put(t_symbol::f_instantiate(L"EXTENSIONS"sv), f_as(GL_EXTENSIONS));
	a_module->f_put(t_symbol::f_instantiate(L"NEAREST"sv), f_as(GL_NEAREST));
	a_module->f_put(t_symbol::f_instantiate(L"LINEAR"sv), f_as(GL_LINEAR));
	a_module->f_put(t_symbol::f_instantiate(L"NEAREST_MIPMAP_NEAREST"sv), f_as(GL_NEAREST_MIPMAP_NEAREST));
	a_module->f_put(t_symbol::f_instantiate(L"LINEAR_MIPMAP_NEAREST"sv), f_as(GL_LINEAR_MIPMAP_NEAREST));
	a_module->f_put(t_symbol::f_instantiate(L"NEAREST_MIPMAP_LINEAR"sv), f_as(GL_NEAREST_MIPMAP_LINEAR));
	a_module->f_put(t_symbol::f_instantiate(L"LINEAR_MIPMAP_LINEAR"sv), f_as(GL_LINEAR_MIPMAP_LINEAR));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_MAG_FILTER"sv), f_as(GL_TEXTURE_MAG_FILTER));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_MIN_FILTER"sv), f_as(GL_TEXTURE_MIN_FILTER));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_WRAP_S"sv), f_as(GL_TEXTURE_WRAP_S));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_WRAP_T"sv), f_as(GL_TEXTURE_WRAP_T));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE"sv), f_as(GL_TEXTURE));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP"sv), f_as(GL_TEXTURE_CUBE_MAP));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_BINDING_CUBE_MAP"sv), f_as(GL_TEXTURE_BINDING_CUBE_MAP));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_POSITIVE_X"sv), f_as(GL_TEXTURE_CUBE_MAP_POSITIVE_X));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_NEGATIVE_X"sv), f_as(GL_TEXTURE_CUBE_MAP_NEGATIVE_X));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_POSITIVE_Y"sv), f_as(GL_TEXTURE_CUBE_MAP_POSITIVE_Y));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_NEGATIVE_Y"sv), f_as(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_POSITIVE_Z"sv), f_as(GL_TEXTURE_CUBE_MAP_POSITIVE_Z));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_NEGATIVE_Z"sv), f_as(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_CUBE_MAP_TEXTURE_SIZE"sv), f_as(GL_MAX_CUBE_MAP_TEXTURE_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE0"sv), f_as(GL_TEXTURE0));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE1"sv), f_as(GL_TEXTURE1));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE2"sv), f_as(GL_TEXTURE2));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE3"sv), f_as(GL_TEXTURE3));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE4"sv), f_as(GL_TEXTURE4));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE5"sv), f_as(GL_TEXTURE5));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE6"sv), f_as(GL_TEXTURE6));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE7"sv), f_as(GL_TEXTURE7));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE8"sv), f_as(GL_TEXTURE8));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE9"sv), f_as(GL_TEXTURE9));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE10"sv), f_as(GL_TEXTURE10));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE11"sv), f_as(GL_TEXTURE11));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE12"sv), f_as(GL_TEXTURE12));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE13"sv), f_as(GL_TEXTURE13));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE14"sv), f_as(GL_TEXTURE14));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE15"sv), f_as(GL_TEXTURE15));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE16"sv), f_as(GL_TEXTURE16));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE17"sv), f_as(GL_TEXTURE17));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE18"sv), f_as(GL_TEXTURE18));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE19"sv), f_as(GL_TEXTURE19));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE20"sv), f_as(GL_TEXTURE20));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE21"sv), f_as(GL_TEXTURE21));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE22"sv), f_as(GL_TEXTURE22));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE23"sv), f_as(GL_TEXTURE23));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE24"sv), f_as(GL_TEXTURE24));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE25"sv), f_as(GL_TEXTURE25));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE26"sv), f_as(GL_TEXTURE26));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE27"sv), f_as(GL_TEXTURE27));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE28"sv), f_as(GL_TEXTURE28));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE29"sv), f_as(GL_TEXTURE29));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE30"sv), f_as(GL_TEXTURE30));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE31"sv), f_as(GL_TEXTURE31));
	a_module->f_put(t_symbol::f_instantiate(L"ACTIVE_TEXTURE"sv), f_as(GL_ACTIVE_TEXTURE));
	a_module->f_put(t_symbol::f_instantiate(L"REPEAT"sv), f_as(GL_REPEAT));
	a_module->f_put(t_symbol::f_instantiate(L"CLAMP_TO_EDGE"sv), f_as(GL_CLAMP_TO_EDGE));
	a_module->f_put(t_symbol::f_instantiate(L"MIRRORED_REPEAT"sv), f_as(GL_MIRRORED_REPEAT));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_VEC2"sv), f_as(GL_FLOAT_VEC2));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_VEC3"sv), f_as(GL_FLOAT_VEC3));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_VEC4"sv), f_as(GL_FLOAT_VEC4));
	a_module->f_put(t_symbol::f_instantiate(L"INT_VEC2"sv), f_as(GL_INT_VEC2));
	a_module->f_put(t_symbol::f_instantiate(L"INT_VEC3"sv), f_as(GL_INT_VEC3));
	a_module->f_put(t_symbol::f_instantiate(L"INT_VEC4"sv), f_as(GL_INT_VEC4));
	a_module->f_put(t_symbol::f_instantiate(L"BOOL"sv), f_as(GL_BOOL));
	a_module->f_put(t_symbol::f_instantiate(L"BOOL_VEC2"sv), f_as(GL_BOOL_VEC2));
	a_module->f_put(t_symbol::f_instantiate(L"BOOL_VEC3"sv), f_as(GL_BOOL_VEC3));
	a_module->f_put(t_symbol::f_instantiate(L"BOOL_VEC4"sv), f_as(GL_BOOL_VEC4));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_MAT2"sv), f_as(GL_FLOAT_MAT2));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_MAT3"sv), f_as(GL_FLOAT_MAT3));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_MAT4"sv), f_as(GL_FLOAT_MAT4));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLER_2D"sv), f_as(GL_SAMPLER_2D));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLER_CUBE"sv), f_as(GL_SAMPLER_CUBE));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_ENABLED"sv), f_as(GL_VERTEX_ATTRIB_ARRAY_ENABLED));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_SIZE"sv), f_as(GL_VERTEX_ATTRIB_ARRAY_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_STRIDE"sv), f_as(GL_VERTEX_ATTRIB_ARRAY_STRIDE));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_TYPE"sv), f_as(GL_VERTEX_ATTRIB_ARRAY_TYPE));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_NORMALIZED"sv), f_as(GL_VERTEX_ATTRIB_ARRAY_NORMALIZED));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_POINTER"sv), f_as(GL_VERTEX_ATTRIB_ARRAY_POINTER));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_BUFFER_BINDING"sv), f_as(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING));
	a_module->f_put(t_symbol::f_instantiate(L"IMPLEMENTATION_COLOR_READ_TYPE"sv), f_as(GL_IMPLEMENTATION_COLOR_READ_TYPE));
	a_module->f_put(t_symbol::f_instantiate(L"IMPLEMENTATION_COLOR_READ_FORMAT"sv), f_as(GL_IMPLEMENTATION_COLOR_READ_FORMAT));
	a_module->f_put(t_symbol::f_instantiate(L"COMPILE_STATUS"sv), f_as(GL_COMPILE_STATUS));
	a_module->f_put(t_symbol::f_instantiate(L"INFO_LOG_LENGTH"sv), f_as(GL_INFO_LOG_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"SHADER_SOURCE_LENGTH"sv), f_as(GL_SHADER_SOURCE_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"SHADER_COMPILER"sv), f_as(GL_SHADER_COMPILER));
//	a_module->f_put(t_symbol::f_instantiate(L"SHADER_BINARY_FORMATS"sv), f_as(GL_SHADER_BINARY_FORMATS));
	a_module->f_put(t_symbol::f_instantiate(L"NUM_SHADER_BINARY_FORMATS"sv), f_as(GL_NUM_SHADER_BINARY_FORMATS));
	a_module->f_put(t_symbol::f_instantiate(L"LOW_FLOAT"sv), f_as(GL_LOW_FLOAT));
	a_module->f_put(t_symbol::f_instantiate(L"MEDIUM_FLOAT"sv), f_as(GL_MEDIUM_FLOAT));
	a_module->f_put(t_symbol::f_instantiate(L"HIGH_FLOAT"sv), f_as(GL_HIGH_FLOAT));
	a_module->f_put(t_symbol::f_instantiate(L"LOW_INT"sv), f_as(GL_LOW_INT));
	a_module->f_put(t_symbol::f_instantiate(L"MEDIUM_INT"sv), f_as(GL_MEDIUM_INT));
	a_module->f_put(t_symbol::f_instantiate(L"HIGH_INT"sv), f_as(GL_HIGH_INT));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER"sv), f_as(GL_FRAMEBUFFER));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER"sv), f_as(GL_RENDERBUFFER));
	a_module->f_put(t_symbol::f_instantiate(L"RGBA4"sv), f_as(GL_RGBA4));
	a_module->f_put(t_symbol::f_instantiate(L"RGB5_A1"sv), f_as(GL_RGB5_A1));
//	a_module->f_put(t_symbol::f_instantiate(L"RGB565"sv), f_as(GL_RGB565));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_COMPONENT16"sv), f_as(GL_DEPTH_COMPONENT16));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_INDEX"sv), f_as(GL_STENCIL_INDEX));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_INDEX8"sv), f_as(GL_STENCIL_INDEX8));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_WIDTH"sv), f_as(GL_RENDERBUFFER_WIDTH));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_HEIGHT"sv), f_as(GL_RENDERBUFFER_HEIGHT));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_INTERNAL_FORMAT"sv), f_as(GL_RENDERBUFFER_INTERNAL_FORMAT));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_RED_SIZE"sv), f_as(GL_RENDERBUFFER_RED_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_GREEN_SIZE"sv), f_as(GL_RENDERBUFFER_GREEN_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_BLUE_SIZE"sv), f_as(GL_RENDERBUFFER_BLUE_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_ALPHA_SIZE"sv), f_as(GL_RENDERBUFFER_ALPHA_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_DEPTH_SIZE"sv), f_as(GL_RENDERBUFFER_DEPTH_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_STENCIL_SIZE"sv), f_as(GL_RENDERBUFFER_STENCIL_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE"sv), f_as(GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_ATTACHMENT_OBJECT_NAME"sv), f_as(GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL"sv), f_as(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE"sv), f_as(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_ATTACHMENT0"sv), f_as(GL_COLOR_ATTACHMENT0));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_ATTACHMENT"sv), f_as(GL_DEPTH_ATTACHMENT));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_ATTACHMENT"sv), f_as(GL_STENCIL_ATTACHMENT));
	a_module->f_put(t_symbol::f_instantiate(L"NONE"sv), f_as(GL_NONE));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_COMPLETE"sv), f_as(GL_FRAMEBUFFER_COMPLETE));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_INCOMPLETE_ATTACHMENT"sv), f_as(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"sv), f_as(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT));
//	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_INCOMPLETE_DIMENSIONS"sv), f_as(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_UNSUPPORTED"sv), f_as(GL_FRAMEBUFFER_UNSUPPORTED));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_BINDING"sv), f_as(GL_FRAMEBUFFER_BINDING));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_BINDING"sv), f_as(GL_RENDERBUFFER_BINDING));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_RENDERBUFFER_SIZE"sv), f_as(GL_MAX_RENDERBUFFER_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"INVALID_FRAMEBUFFER_OPERATION"sv), f_as(GL_INVALID_FRAMEBUFFER_OPERATION));
}

void t_extension::f_scan(t_scan a_scan)
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

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::gl::t_extension(a_module);
}

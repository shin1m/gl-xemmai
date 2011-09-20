#include "array.h"
#include "buffer.h"
#include "framebuffer.h"
#include "renderbuffer.h"
#include "texture.h"
#include "program.h"

namespace gl
{

namespace xemmai
{

using ::xemmai::t_symbol;
using ::xemmai::f_define;
using ::xemmai::portable::t_scoped_lock;

t_transfer f_tuple(const t_transfer& a_0, const t_transfer& a_1, const t_transfer& a_2)
{
	t_transfer p = t_tuple::f_instantiate(3);
	t_tuple& tuple = f_as<t_tuple&>(p);
	tuple[0].f_construct(a_0);
	tuple[1].f_construct(a_1);
	tuple[2].f_construct(a_2);
	return p;
}

t_transfer f_tuple(const t_transfer& a_0, const t_transfer& a_1, const t_transfer& a_2, const t_transfer& a_3)
{
	t_transfer p = t_tuple::f_instantiate(4);
	t_tuple& tuple = f_as<t_tuple&>(p);
	tuple[0].f_construct(a_0);
	tuple[1].f_construct(a_1);
	tuple[2].f_construct(a_2);
	tuple[3].f_construct(a_3);
	return p;
}

t_mutex t_session::v_mutex;
bool t_session::v_running = false;
XEMMAI__PORTABLE__THREAD t_session* t_session::v_instance;

t_session::t_session(t_extension* a_extension) : v_extension(a_extension)
{
	t_scoped_lock lock(v_mutex);
	if (v_running) t_throwable::f_throw(L"main already running.");
	v_running = true;
	v_instance = this;
}

t_session::~t_session()
{
	while (!v_buffers.empty()) f_as<t_buffer&>(v_buffers.begin()->second).f_delete();
	while (!v_framebuffers.empty()) f_as<t_framebuffer&>(v_framebuffers.begin()->second).f_delete();
	while (!v_renderbuffers.empty()) f_as<t_renderbuffer&>(v_renderbuffers.begin()->second).f_delete();
	while (!v_textures.empty()) f_as<t_texture&>(v_textures.begin()->second).f_delete();
	while (!v_programs.empty()) f_as<t_program&>(v_programs.begin()->second).f_delete();
	while (!v_shaders.empty()) f_as<t_shader&>(v_shaders.begin()->second).f_delete();
	t_scoped_lock lock(v_mutex);
	v_running = false;
	v_instance = 0;
}

namespace
{

void f_main(t_extension* a_extension, const t_value& a_callable)
{
	t_session session(a_extension);
	a_callable();
}

void f_bind_buffer(GLenum a_target, const t_buffer* a_buffer)
{
	glBindBuffer(a_target, a_buffer ? a_buffer->f_id() : 0);
}

void f_bind_framebuffer(GLenum a_target, const t_framebuffer* a_framebuffer)
{
	glBindFramebuffer(a_target, a_framebuffer ? a_framebuffer->f_id() : 0);
}

void f_bind_renderbuffer(GLenum a_target, const t_renderbuffer* a_renderbuffer)
{
	glBindRenderbuffer(a_target, a_renderbuffer ? a_renderbuffer->f_id() : 0);
}

void f_bind_texture(GLenum a_target, const t_texture* a_texture)
{
	glBindTexture(a_target, a_texture ? a_texture->f_id() : 0);
}

void f_buffer_data(GLenum a_target, const t_bytes& a_data, GLenum a_usage)
{
	glBufferData(a_target, a_data.f_size(), &a_data[0], a_usage);
}

void f_buffer_sub_data(GLenum a_target, GLintptr a_offset, const t_bytes& a_data)
{
	glBufferSubData(a_target, a_offset, a_data.f_size(), &a_data[0]);
}

void f_color_mask(bool a_red, bool a_green, bool a_blue, bool a_alpha)
{
	glColorMask(a_red ? GL_TRUE : GL_FALSE, a_green ? GL_TRUE : GL_FALSE, a_blue ? GL_TRUE : GL_FALSE, a_alpha ? GL_TRUE : GL_FALSE);
}

void f_depth_mask(bool a_flag)
{
	glDepthMask(a_flag ? GL_TRUE : GL_FALSE);
}

void f_draw_elements(GLenum a_mode, GLsizei a_count, GLenum a_type, GLintptr a_offset)
{
	glDrawElements(a_mode, a_count, a_type, reinterpret_cast<const GLvoid*>(a_offset));
}

void f_framebuffer_renderbuffer(GLenum a_target, GLenum a_attachment, GLenum a_renderbuffertarget, const t_renderbuffer* a_renderbuffer)
{
	glFramebufferRenderbuffer(a_target, a_attachment, a_renderbuffertarget, a_renderbuffer ? a_renderbuffer->f_id() : 0);
}

void f_framebuffer_texture2d(GLenum a_target, GLenum a_attachment, GLenum a_textarget, const t_texture* a_texture, GLint a_level)
{
	glFramebufferTexture2D(a_target, a_attachment, a_textarget, a_texture ? a_texture->f_id() : 0, a_level);
}

bool f_get_boolean(GLenum a_name)
{
	GLboolean values[4];
	glGetBooleanv(a_name, values);
	return values[0] != GL_FALSE;
}

t_transfer f_get_booleans(GLenum a_name)
{
	GLboolean values[4];
	glGetBooleanv(a_name, values);
	return f_tuple(t_transfer(values[0] != GL_FALSE), t_transfer(values[1] != GL_FALSE), t_transfer(values[2] != GL_FALSE), t_transfer(values[3] != GL_FALSE));
}

GLint f_get_buffer_parameter(GLenum a_target, GLenum a_name)
{
	GLint value;
	glGetBufferParameteriv(a_target, a_name, &value);
	return value;
}

GLfloat f_get_float(GLenum a_name)
{
	GLfloat values[4];
	glGetFloatv(a_name, values);
	return values[0];
}

t_transfer f_get_floats(GLenum a_name)
{
	GLfloat values[4];
	glGetFloatv(a_name, values);
	return f_tuple(t_transfer(values[0]), t_transfer(values[1]), t_transfer(values[2]), t_transfer(values[3]));
}

GLint f_get_framebuffer_attachment_parameter(GLenum a_target, GLenum a_attachment, GLenum a_name)
{
	GLint value;
	glGetFramebufferAttachmentParameteriv(a_target, a_attachment, a_name, &value);
	return value;
}

GLint f_get_integer(GLenum a_name)
{
	GLint values[4];
	glGetIntegerv(a_name, values);
	return values[0];
}

t_transfer f_get_integers(GLenum a_name)
{
	GLint values[4];
	glGetIntegerv(a_name, values);
	return f_tuple(t_transfer(values[0]), t_transfer(values[1]), t_transfer(values[2]), t_transfer(values[3]));
}

GLint f_get_renderbuffer_parameter(GLenum a_target, GLenum a_name)
{
	GLint value;
	glGetRenderbufferParameteriv(a_target, a_name, &value);
	return value;
}

t_transfer f_get_shader_precision_format(GLenum a_shader, GLenum a_precision)
{
	GLint range[2];
	GLint precision;
	glGetShaderPrecisionFormat(a_shader, a_precision, range, &precision);
	return f_tuple(t_transfer(range[0]), t_transfer(range[1]), t_transfer(precision));
}

std::wstring f_get_string(GLenum a_name)
{
	return f_convert(reinterpret_cast<const char*>(glGetString(a_name)));
}

GLfloat f_get_tex_parameterf(GLenum a_target, GLenum a_name)
{
	GLfloat values[4];
	glGetTexParameterfv(a_target, a_name, values);
	return values[0];
}

t_transfer f_get_tex_parameterfv(GLenum a_target, GLenum a_name)
{
	GLfloat values[4];
	glGetTexParameterfv(a_target, a_name, values);
	return f_tuple(t_transfer(values[0]), t_transfer(values[1]), t_transfer(values[2]), t_transfer(values[3]));
}

GLint f_get_tex_parameteri(GLenum a_target, GLenum a_name)
{
	GLint values[4];
	glGetTexParameteriv(a_target, a_name, values);
	return values[0];
}

t_transfer f_get_tex_parameteriv(GLenum a_target, GLenum a_name)
{
	GLint values[4];
	glGetTexParameteriv(a_target, a_name, values);
	return f_tuple(t_transfer(values[0]), t_transfer(values[1]), t_transfer(values[2]), t_transfer(values[3]));
}

GLfloat f_get_vertex_attribf(GLenum a_target, GLenum a_name)
{
	GLfloat values[4];
	glGetVertexAttribfv(a_target, a_name, values);
	return values[0];
}

t_transfer f_get_vertex_attribfv(GLenum a_target, GLenum a_name)
{
	GLfloat values[4];
	glGetVertexAttribfv(a_target, a_name, values);
	return f_tuple(t_transfer(values[0]), t_transfer(values[1]), t_transfer(values[2]), t_transfer(values[3]));
}

GLint f_get_vertex_attribi(GLenum a_target, GLenum a_name)
{
	GLint values[4];
	glGetVertexAttribiv(a_target, a_name, values);
	return values[0];
}

t_transfer f_get_vertex_attribiv(GLenum a_target, GLenum a_name)
{
	GLint values[4];
	glGetVertexAttribiv(a_target, a_name, values);
	return f_tuple(t_transfer(values[0]), t_transfer(values[1]), t_transfer(values[2]), t_transfer(values[3]));
}

GLintptr f_get_vertex_attrib_pointer(GLuint a_index, GLenum a_name)
{
	GLvoid* pointer;
	glGetVertexAttribPointerv(a_index, a_name, &pointer);
	return reinterpret_cast<GLintptr>(pointer);
}

bool f_is_enabled(GLenum a_capability)
{
	return glIsEnabled(a_capability) != GL_FALSE;
}

void f_read_pixels(GLint a_x, GLint a_y, GLsizei a_width, GLsizei a_height, GLenum a_format, GLenum a_type, t_bytes& a_data)
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
}

void f_sample_coverage(GLclampf a_value, bool a_invert)
{
	glSampleCoverage(a_value, a_invert ? GL_TRUE : GL_FALSE);
}

void f_tex_image2d(GLenum a_target, GLint a_level, GLint a_internalformat, GLsizei a_width, GLsizei a_height, GLint a_border, GLenum a_format, GLenum a_type, const t_bytes& a_pixels)
{
	glTexImage2D(a_target, a_level, a_internalformat, a_width, a_height, a_border, a_format, a_type, &a_pixels[0]);
}

void f_tex_parameter4f(GLenum a_target, GLenum a_name, GLfloat a_value0, GLfloat a_value1, GLfloat a_value2, GLfloat a_value3)
{
	GLfloat values[] = {a_value0, a_value1, a_value2, a_value3};
	glTexParameterfv(a_target, a_name, values);
}

void f_tex_parameter4i(GLenum a_target, GLenum a_name, GLint a_value0, GLint a_value1, GLint a_value2, GLint a_value3)
{
	GLint values[] = {a_value0, a_value1, a_value2, a_value3};
	glTexParameteriv(a_target, a_name, values);
}

void f_tex_sub_image2d(GLenum a_target, GLint a_level, GLint a_xoffset, GLint a_yoffset, GLsizei a_width, GLsizei a_height, GLenum a_format, GLenum a_type, const t_bytes& a_pixels)
{
	glTexSubImage2D(a_target, a_level, a_xoffset, a_yoffset, a_width, a_height, a_format, a_type, &a_pixels[0]);
}

void f_vertex_attrib_pointer(GLuint a_index, GLint a_size, GLenum a_type, bool a_normalized, GLsizei a_stride, GLintptr a_offset)
{
	glVertexAttribPointer(a_index, a_size, a_type, a_normalized ? GL_TRUE : GL_FALSE, a_stride, reinterpret_cast<const GLvoid*>(a_offset));
}

void f_use_program(const t_program* a_program)
{
	glUseProgram(a_program ? a_program->f_id() : 0);
}

}

t_extension::t_extension(t_object* a_module) : ::xemmai::t_extension(a_module)
{
	t_type_of<t_array_of<short> >::f_define(this, L"Int16Array");
	t_type_of<t_array_of<int> >::f_define(this, L"Int32Array");
	t_type_of<t_array_of<float> >::f_define(this, L"Float32Array");
	t_type_of<t_error>::f_define(this);
	t_type_of<t_buffer>::f_define(this);
	t_type_of<t_framebuffer>::f_define(this);
	t_type_of<t_renderbuffer>::f_define(this);
	t_type_of<t_texture>::f_define(this);
	t_type_of<t_program>::f_define(this);
	t_type_of<t_shader>::f_define(this);
	t_type_of<t_uniform_location>::f_define(this);
	f_define<void (*)(t_extension*, const t_value&), f_main>(this, L"main");
	f_define<void (*)(GLenum), glActiveTexture>(this, L"active_texture");
	f_define<void (*)(GLenum, const t_buffer*), f_bind_buffer>(this, L"bind_buffer");
	f_define<void (*)(GLenum, const t_framebuffer*), f_bind_framebuffer>(this, L"bind_framebuffer");
	f_define<void (*)(GLenum, const t_renderbuffer*), f_bind_renderbuffer>(this, L"bind_renderbuffer");
	f_define<void (*)(GLenum, const t_texture*), f_bind_texture>(this, L"bind_texture");
	f_define<void (*)(GLclampf, GLclampf, GLclampf, GLclampf), glBlendColor>(this, L"blend_color");
	f_define<void (*)(GLenum), glBlendEquation>(this, L"blend_equation");
	f_define<void (*)(GLenum, GLenum), glBlendEquationSeparate>(this, L"blend_equation_separate");
	f_define<void (*)(GLenum, GLenum), glBlendFunc>(this, L"blend_func");
	f_define<void (*)(GLenum, GLenum, GLenum, GLenum), glBlendFuncSeparate>(this, L"blend_func_separate");
	f_define<void (*)(GLenum, const t_bytes&, GLenum), f_buffer_data>(this, L"buffer_data");
	f_define<void (*)(GLenum, GLintptr, const t_bytes&), f_buffer_sub_data>(this, L"buffer_sub_data");
	f_define<GLenum (*)(GLenum), glCheckFramebufferStatus>(this, L"check_framebuffer_status");
	f_define<void (*)(GLbitfield), glClear>(this, L"clear");
	f_define<void (*)(GLclampf, GLclampf, GLclampf, GLclampf), glClearColor>(this, L"clear_color");
	f_define<void (*)(GLclampf), glClearDepthf>(this, L"clear_depthf");
	f_define<void (*)(GLint), glClearStencil>(this, L"clear_stencil");
	f_define<void (*)(bool, bool, bool, bool), f_color_mask>(this, L"color_mask");
	f_define<void (*)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint), glCopyTexImage2D>(this, L"copy_tex_image2d");
	f_define<void (*)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei), glCopyTexSubImage2D>(this, L"copy_tex_sub_image2d");
	f_define<void (*)(GLenum), glCullFace>(this, L"cull_face");
	f_define<void (*)(GLenum), glDepthFunc>(this, L"depth_func");
	f_define<void (*)(bool), f_depth_mask>(this, L"depth_mask");
	f_define<void (*)(GLclampf, GLclampf), glDepthRangef>(this, L"depth_rangef");
	f_define<void (*)(GLenum), glDisable>(this, L"disable");
	f_define<void (*)(GLuint), glDisableVertexAttribArray>(this, L"disable_vertex_attrib_array");
	f_define<void (*)(GLenum, GLint, GLsizei), glDrawArrays>(this, L"draw_arrays");
	f_define<void (*)(GLenum, GLsizei, GLenum, GLintptr), f_draw_elements>(this, L"draw_elements");
	f_define<void (*)(GLenum), glEnable>(this, L"enable");
	f_define<void (*)(GLuint), glEnableVertexAttribArray>(this, L"enable_vertex_attrib_array");
	f_define<void (*)(), glFinish>(this, L"finish");
	f_define<void (*)(), glFlush>(this, L"flush");
	f_define<void (*)(GLenum, GLenum, GLenum, const t_renderbuffer*), f_framebuffer_renderbuffer>(this, L"framebuffer_renderbuffer");
	f_define<void (*)(GLenum, GLenum, GLenum, const t_texture*, GLint), f_framebuffer_texture2d>(this, L"framebuffer_texture2d");
	f_define<void (*)(GLenum), glFrontFace>(this, L"front_face");
	f_define<void (*)(GLenum), glGenerateMipmap>(this, L"generate_mipmap");
	f_define<bool (*)(GLenum), f_get_boolean>(this, L"get_boolean");
	f_define<t_transfer (*)(GLenum), f_get_booleans>(this, L"get_booleans");
	f_define<GLint (*)(GLenum, GLenum), f_get_buffer_parameter>(this, L"get_buffer_parameter");
	f_define<GLfloat (*)(GLenum), f_get_float>(this, L"get_float");
	f_define<t_transfer (*)(GLenum), f_get_floats>(this, L"get_floats");
	f_define<GLenum (*)(), glGetError>(this, L"get_error");
	f_define<GLint (*)(GLenum, GLenum, GLenum), f_get_framebuffer_attachment_parameter>(this, L"get_framebuffer_attachment_parameter");
	f_define<GLint (*)(GLenum), f_get_integer>(this, L"get_integer");
	f_define<t_transfer (*)(GLenum), f_get_integers>(this, L"get_integers");
	f_define<GLint (*)(GLenum, GLenum), f_get_renderbuffer_parameter>(this, L"get_renderbuffer_parameter");
	f_define<t_transfer (*)(GLenum, GLenum), f_get_shader_precision_format>(this, L"get_shader_precision_format");
	f_define<std::wstring (*)(GLenum), f_get_string>(this, L"get_string");
	f_define<GLfloat (*)(GLenum, GLenum), f_get_tex_parameterf>(this, L"get_tex_parameterf");
	f_define<t_transfer (*)(GLenum, GLenum), f_get_tex_parameterfv>(this, L"get_tex_parameterfv");
	f_define<GLint (*)(GLenum, GLenum), f_get_tex_parameteri>(this, L"get_tex_parameteri");
	f_define<t_transfer (*)(GLenum, GLenum), f_get_tex_parameteriv>(this, L"get_tex_parameteriv");
	f_define<GLfloat (*)(GLuint, GLenum), f_get_vertex_attribf>(this, L"get_vertex_attribf");
	f_define<t_transfer (*)(GLuint, GLenum), f_get_vertex_attribfv>(this, L"get_vertex_attribfv");
	f_define<GLint (*)(GLuint, GLenum), f_get_vertex_attribi>(this, L"get_vertex_attribi");
	f_define<t_transfer (*)(GLuint, GLenum), f_get_vertex_attribiv>(this, L"get_vertex_attribiv");
	f_define<GLintptr (*)(GLuint, GLenum), f_get_vertex_attrib_pointer>(this, L"get_vertex_attrib_pointer");
	f_define<void (*)(GLenum, GLenum), glHint>(this, L"hint");
	f_define<bool (*)(GLenum), f_is_enabled>(this, L"is_enabled");
	f_define<void (*)(GLfloat), glLineWidth>(this, L"line_width");
	f_define<void (*)(GLenum, GLint), glPixelStorei>(this, L"pixel_storei");
	f_define<void (*)(GLfloat, GLfloat), glPolygonOffset>(this, L"polygon_offset");
	f_define<void (*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, t_bytes&), f_read_pixels>(this, L"read_pixels");
	f_define<void (*)(), glReleaseShaderCompiler>(this, L"release_shader_compiler");
	f_define<void (*)(GLenum, GLenum, GLsizei, GLsizei), glRenderbufferStorage>(this, L"renderbuffer_storage");
	f_define<void (*)(GLclampf, bool), f_sample_coverage>(this, L"sample_coverage");
	f_define<void (*)(GLint, GLint, GLsizei, GLsizei), glScissor>(this, L"scissor");
	f_define<void (*)(GLenum, GLint, GLuint), glStencilFunc>(this, L"stencil_func");
	f_define<void (*)(GLenum, GLenum, GLint, GLuint), glStencilFuncSeparate>(this, L"stencil_func_separate");
	f_define<void (*)(GLuint), glStencilMask>(this, L"stencil_mask");
	f_define<void (*)(GLenum, GLuint), glStencilMaskSeparate>(this, L"stencil_mask_separate");
	f_define<void (*)(GLenum, GLenum, GLenum), glStencilOp>(this, L"stencil_op");
	f_define<void (*)(GLenum, GLenum, GLenum, GLenum), glStencilOpSeparate>(this, L"stencil_op_separate");
	f_define<void (*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const t_bytes&), f_tex_image2d>(this, L"tex_image2d");
	f_define<void (*)(GLenum, GLenum, GLfloat), glTexParameterf>(this, L"tex_parameterf");
	f_define<void (*)(GLenum, GLenum, GLfloat, GLfloat, GLfloat, GLfloat), f_tex_parameter4f>(this, L"tex_parameter4f");
	f_define<void (*)(GLenum, GLenum, GLint), glTexParameteri>(this, L"tex_parameteri");
	f_define<void (*)(GLenum, GLenum, GLint, GLint, GLint, GLint), f_tex_parameter4i>(this, L"tex_parameter4i");
	f_define<void (*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const t_bytes&), f_tex_sub_image2d>(this, L"tex_sub_image2d");
	f_define<void (*)(const t_program*), f_use_program>(this, L"use_program");
	f_define<void (*)(GLuint, GLfloat), glVertexAttrib1f>(this, L"vertex_attrib1f");
	f_define<void (*)(GLuint, GLfloat, GLfloat), glVertexAttrib2f>(this, L"vertex_attrib2f");
	f_define<void (*)(GLuint, GLfloat, GLfloat, GLfloat), glVertexAttrib3f>(this, L"vertex_attrib3f");
	f_define<void (*)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat), glVertexAttrib4f>(this, L"vertex_attrib4f");
	f_define<void (*)(GLuint, GLint, GLenum, bool, GLsizei, GLintptr), f_vertex_attrib_pointer>(this, L"vertex_attrib_pointer");
	f_define<void (*)(GLint, GLint, GLsizei, GLsizei), glViewport>(this, L"viewport");
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_BUFFER_BIT"), f_as(GL_DEPTH_BUFFER_BIT));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BUFFER_BIT"), f_as(GL_STENCIL_BUFFER_BIT));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_BUFFER_BIT"), f_as(GL_COLOR_BUFFER_BIT));
	a_module->f_put(t_symbol::f_instantiate(L"FALSE"), f_as(GL_FALSE));
	a_module->f_put(t_symbol::f_instantiate(L"TRUE"), f_as(GL_TRUE));
	a_module->f_put(t_symbol::f_instantiate(L"POINTS"), f_as(GL_POINTS));
	a_module->f_put(t_symbol::f_instantiate(L"LINES"), f_as(GL_LINES));
	a_module->f_put(t_symbol::f_instantiate(L"LINE_LOOP"), f_as(GL_LINE_LOOP));
	a_module->f_put(t_symbol::f_instantiate(L"LINE_STRIP"), f_as(GL_LINE_STRIP));
	a_module->f_put(t_symbol::f_instantiate(L"TRIANGLES"), f_as(GL_TRIANGLES));
	a_module->f_put(t_symbol::f_instantiate(L"TRIANGLE_STRIP"), f_as(GL_TRIANGLE_STRIP));
	a_module->f_put(t_symbol::f_instantiate(L"TRIANGLE_FAN"), f_as(GL_TRIANGLE_FAN));
	a_module->f_put(t_symbol::f_instantiate(L"ZERO"), f_as(GL_ZERO));
	a_module->f_put(t_symbol::f_instantiate(L"ONE"), f_as(GL_ONE));
	a_module->f_put(t_symbol::f_instantiate(L"SRC_COLOR"), f_as(GL_SRC_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_SRC_COLOR"), f_as(GL_ONE_MINUS_SRC_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"SRC_ALPHA"), f_as(GL_SRC_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_SRC_ALPHA"), f_as(GL_ONE_MINUS_SRC_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"DST_ALPHA"), f_as(GL_DST_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_DST_ALPHA"), f_as(GL_ONE_MINUS_DST_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"DST_COLOR"), f_as(GL_DST_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_DST_COLOR"), f_as(GL_ONE_MINUS_DST_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"SRC_ALPHA_SATURATE"), f_as(GL_SRC_ALPHA_SATURATE));
	a_module->f_put(t_symbol::f_instantiate(L"FUNC_ADD"), f_as(GL_FUNC_ADD));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_EQUATION"), f_as(GL_BLEND_EQUATION));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_EQUATION_RGB"), f_as(GL_BLEND_EQUATION_RGB));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_EQUATION_ALPHA"), f_as(GL_BLEND_EQUATION_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"FUNC_SUBTRACT"), f_as(GL_FUNC_SUBTRACT));
	a_module->f_put(t_symbol::f_instantiate(L"FUNC_REVERSE_SUBTRACT"), f_as(GL_FUNC_REVERSE_SUBTRACT));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_DST_RGB"), f_as(GL_BLEND_DST_RGB));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_SRC_RGB"), f_as(GL_BLEND_SRC_RGB));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_DST_ALPHA"), f_as(GL_BLEND_DST_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_SRC_ALPHA"), f_as(GL_BLEND_SRC_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"CONSTANT_COLOR"), f_as(GL_CONSTANT_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_CONSTANT_COLOR"), f_as(GL_ONE_MINUS_CONSTANT_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"CONSTANT_ALPHA"), f_as(GL_CONSTANT_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"ONE_MINUS_CONSTANT_ALPHA"), f_as(GL_ONE_MINUS_CONSTANT_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND_COLOR"), f_as(GL_BLEND_COLOR));
	a_module->f_put(t_symbol::f_instantiate(L"ARRAY_BUFFER"), f_as(GL_ARRAY_BUFFER));
	a_module->f_put(t_symbol::f_instantiate(L"ELEMENT_ARRAY_BUFFER"), f_as(GL_ELEMENT_ARRAY_BUFFER));
	a_module->f_put(t_symbol::f_instantiate(L"ARRAY_BUFFER_BINDING"), f_as(GL_ARRAY_BUFFER_BINDING));
	a_module->f_put(t_symbol::f_instantiate(L"ELEMENT_ARRAY_BUFFER_BINDING"), f_as(GL_ELEMENT_ARRAY_BUFFER_BINDING));
	a_module->f_put(t_symbol::f_instantiate(L"STREAM_DRAW"), f_as(GL_STREAM_DRAW));
	a_module->f_put(t_symbol::f_instantiate(L"STATIC_DRAW"), f_as(GL_STATIC_DRAW));
	a_module->f_put(t_symbol::f_instantiate(L"DYNAMIC_DRAW"), f_as(GL_DYNAMIC_DRAW));
	a_module->f_put(t_symbol::f_instantiate(L"BUFFER_SIZE"), f_as(GL_BUFFER_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"BUFFER_USAGE"), f_as(GL_BUFFER_USAGE));
	a_module->f_put(t_symbol::f_instantiate(L"CURRENT_VERTEX_ATTRIB"), f_as(GL_CURRENT_VERTEX_ATTRIB));
	a_module->f_put(t_symbol::f_instantiate(L"FRONT"), f_as(GL_FRONT));
	a_module->f_put(t_symbol::f_instantiate(L"BACK"), f_as(GL_BACK));
	a_module->f_put(t_symbol::f_instantiate(L"FRONT_AND_BACK"), f_as(GL_FRONT_AND_BACK));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_2D"), f_as(GL_TEXTURE_2D));
	a_module->f_put(t_symbol::f_instantiate(L"CULL_FACE"), f_as(GL_CULL_FACE));
	a_module->f_put(t_symbol::f_instantiate(L"BLEND"), f_as(GL_BLEND));
	a_module->f_put(t_symbol::f_instantiate(L"DITHER"), f_as(GL_DITHER));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_TEST"), f_as(GL_STENCIL_TEST));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_TEST"), f_as(GL_DEPTH_TEST));
	a_module->f_put(t_symbol::f_instantiate(L"SCISSOR_TEST"), f_as(GL_SCISSOR_TEST));
	a_module->f_put(t_symbol::f_instantiate(L"POLYGON_OFFSET_FILL"), f_as(GL_POLYGON_OFFSET_FILL));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLE_ALPHA_TO_COVERAGE"), f_as(GL_SAMPLE_ALPHA_TO_COVERAGE));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLE_COVERAGE"), f_as(GL_SAMPLE_COVERAGE));
	a_module->f_put(t_symbol::f_instantiate(L"NO_ERROR"), f_as(GL_NO_ERROR));
	a_module->f_put(t_symbol::f_instantiate(L"INVALID_ENUM"), f_as(GL_INVALID_ENUM));
	a_module->f_put(t_symbol::f_instantiate(L"INVALID_VALUE"), f_as(GL_INVALID_VALUE));
	a_module->f_put(t_symbol::f_instantiate(L"INVALID_OPERATION"), f_as(GL_INVALID_OPERATION));
	a_module->f_put(t_symbol::f_instantiate(L"OUT_OF_MEMORY"), f_as(GL_OUT_OF_MEMORY));
	a_module->f_put(t_symbol::f_instantiate(L"CW"), f_as(GL_CW));
	a_module->f_put(t_symbol::f_instantiate(L"CCW"), f_as(GL_CCW));
	a_module->f_put(t_symbol::f_instantiate(L"LINE_WIDTH"), f_as(GL_LINE_WIDTH));
	a_module->f_put(t_symbol::f_instantiate(L"ALIASED_POINT_SIZE_RANGE"), f_as(GL_ALIASED_POINT_SIZE_RANGE));
	a_module->f_put(t_symbol::f_instantiate(L"ALIASED_LINE_WIDTH_RANGE"), f_as(GL_ALIASED_LINE_WIDTH_RANGE));
	a_module->f_put(t_symbol::f_instantiate(L"CULL_FACE_MODE"), f_as(GL_CULL_FACE_MODE));
	a_module->f_put(t_symbol::f_instantiate(L"FRONT_FACE"), f_as(GL_FRONT_FACE));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_RANGE"), f_as(GL_DEPTH_RANGE));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_WRITEMASK"), f_as(GL_DEPTH_WRITEMASK));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_CLEAR_VALUE"), f_as(GL_DEPTH_CLEAR_VALUE));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_FUNC"), f_as(GL_DEPTH_FUNC));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_CLEAR_VALUE"), f_as(GL_STENCIL_CLEAR_VALUE));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_FUNC"), f_as(GL_STENCIL_FUNC));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_FAIL"), f_as(GL_STENCIL_FAIL));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_PASS_DEPTH_FAIL"), f_as(GL_STENCIL_PASS_DEPTH_FAIL));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_PASS_DEPTH_PASS"), f_as(GL_STENCIL_PASS_DEPTH_PASS));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_REF"), f_as(GL_STENCIL_REF));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_VALUE_MASK"), f_as(GL_STENCIL_VALUE_MASK));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_WRITEMASK"), f_as(GL_STENCIL_WRITEMASK));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_FUNC"), f_as(GL_STENCIL_BACK_FUNC));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_FAIL"), f_as(GL_STENCIL_BACK_FAIL));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_PASS_DEPTH_FAIL"), f_as(GL_STENCIL_BACK_PASS_DEPTH_FAIL));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_PASS_DEPTH_PASS"), f_as(GL_STENCIL_BACK_PASS_DEPTH_PASS));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_REF"), f_as(GL_STENCIL_BACK_REF));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_VALUE_MASK"), f_as(GL_STENCIL_BACK_VALUE_MASK));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BACK_WRITEMASK"), f_as(GL_STENCIL_BACK_WRITEMASK));
	a_module->f_put(t_symbol::f_instantiate(L"VIEWPORT"), f_as(GL_VIEWPORT));
	a_module->f_put(t_symbol::f_instantiate(L"SCISSOR_BOX"), f_as(GL_SCISSOR_BOX));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_CLEAR_VALUE"), f_as(GL_COLOR_CLEAR_VALUE));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_WRITEMASK"), f_as(GL_COLOR_WRITEMASK));
	a_module->f_put(t_symbol::f_instantiate(L"UNPACK_ALIGNMENT"), f_as(GL_UNPACK_ALIGNMENT));
	a_module->f_put(t_symbol::f_instantiate(L"PACK_ALIGNMENT"), f_as(GL_PACK_ALIGNMENT));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_TEXTURE_SIZE"), f_as(GL_MAX_TEXTURE_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_VIEWPORT_DIMS"), f_as(GL_MAX_VIEWPORT_DIMS));
	a_module->f_put(t_symbol::f_instantiate(L"SUBPIXEL_BITS"), f_as(GL_SUBPIXEL_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"RED_BITS"), f_as(GL_RED_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"GREEN_BITS"), f_as(GL_GREEN_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"BLUE_BITS"), f_as(GL_BLUE_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"ALPHA_BITS"), f_as(GL_ALPHA_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_BITS"), f_as(GL_DEPTH_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_BITS"), f_as(GL_STENCIL_BITS));
	a_module->f_put(t_symbol::f_instantiate(L"POLYGON_OFFSET_UNITS"), f_as(GL_POLYGON_OFFSET_UNITS));
	a_module->f_put(t_symbol::f_instantiate(L"POLYGON_OFFSET_FACTOR"), f_as(GL_POLYGON_OFFSET_FACTOR));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_BINDING_2D"), f_as(GL_TEXTURE_BINDING_2D));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLE_BUFFERS"), f_as(GL_SAMPLE_BUFFERS));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLES"), f_as(GL_SAMPLES));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLE_COVERAGE_VALUE"), f_as(GL_SAMPLE_COVERAGE_VALUE));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLE_COVERAGE_INVERT"), f_as(GL_SAMPLE_COVERAGE_INVERT));
	a_module->f_put(t_symbol::f_instantiate(L"NUM_COMPRESSED_TEXTURE_FORMATS"), f_as(GL_NUM_COMPRESSED_TEXTURE_FORMATS));
	a_module->f_put(t_symbol::f_instantiate(L"COMPRESSED_TEXTURE_FORMATS"), f_as(GL_COMPRESSED_TEXTURE_FORMATS));
	a_module->f_put(t_symbol::f_instantiate(L"DONT_CARE"), f_as(GL_DONT_CARE));
	a_module->f_put(t_symbol::f_instantiate(L"FASTEST"), f_as(GL_FASTEST));
	a_module->f_put(t_symbol::f_instantiate(L"NICEST"), f_as(GL_NICEST));
	a_module->f_put(t_symbol::f_instantiate(L"GENERATE_MIPMAP_HINT"), f_as(GL_GENERATE_MIPMAP_HINT));
	a_module->f_put(t_symbol::f_instantiate(L"BYTE"), f_as(GL_BYTE));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_BYTE"), f_as(GL_UNSIGNED_BYTE));
	a_module->f_put(t_symbol::f_instantiate(L"SHORT"), f_as(GL_SHORT));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_SHORT"), f_as(GL_UNSIGNED_SHORT));
	a_module->f_put(t_symbol::f_instantiate(L"INT"), f_as(GL_INT));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_INT"), f_as(GL_UNSIGNED_INT));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT"), f_as(GL_FLOAT));
	a_module->f_put(t_symbol::f_instantiate(L"FIXED"), f_as(GL_FIXED));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_COMPONENT"), f_as(GL_DEPTH_COMPONENT));
	a_module->f_put(t_symbol::f_instantiate(L"ALPHA"), f_as(GL_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"RGB"), f_as(GL_RGB));
	a_module->f_put(t_symbol::f_instantiate(L"RGBA"), f_as(GL_RGBA));
	a_module->f_put(t_symbol::f_instantiate(L"LUMINANCE"), f_as(GL_LUMINANCE));
	a_module->f_put(t_symbol::f_instantiate(L"LUMINANCE_ALPHA"), f_as(GL_LUMINANCE_ALPHA));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_SHORT_4_4_4_4"), f_as(GL_UNSIGNED_SHORT_4_4_4_4));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_SHORT_5_5_5_1"), f_as(GL_UNSIGNED_SHORT_5_5_5_1));
	a_module->f_put(t_symbol::f_instantiate(L"UNSIGNED_SHORT_5_6_5"), f_as(GL_UNSIGNED_SHORT_5_6_5));
	a_module->f_put(t_symbol::f_instantiate(L"FRAGMENT_SHADER"), f_as(GL_FRAGMENT_SHADER));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_SHADER"), f_as(GL_VERTEX_SHADER));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_VERTEX_ATTRIBS"), f_as(GL_MAX_VERTEX_ATTRIBS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_VERTEX_UNIFORM_VECTORS"), f_as(GL_MAX_VERTEX_UNIFORM_VECTORS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_VARYING_VECTORS"), f_as(GL_MAX_VARYING_VECTORS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_COMBINED_TEXTURE_IMAGE_UNITS"), f_as(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_VERTEX_TEXTURE_IMAGE_UNITS"), f_as(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_TEXTURE_IMAGE_UNITS"), f_as(GL_MAX_TEXTURE_IMAGE_UNITS));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_FRAGMENT_UNIFORM_VECTORS"), f_as(GL_MAX_FRAGMENT_UNIFORM_VECTORS));
	a_module->f_put(t_symbol::f_instantiate(L"SHADER_TYPE"), f_as(GL_SHADER_TYPE));
	a_module->f_put(t_symbol::f_instantiate(L"DELETE_STATUS"), f_as(GL_DELETE_STATUS));
	a_module->f_put(t_symbol::f_instantiate(L"LINK_STATUS"), f_as(GL_LINK_STATUS));
	a_module->f_put(t_symbol::f_instantiate(L"VALIDATE_STATUS"), f_as(GL_VALIDATE_STATUS));
	a_module->f_put(t_symbol::f_instantiate(L"ATTACHED_SHADERS"), f_as(GL_ATTACHED_SHADERS));
	a_module->f_put(t_symbol::f_instantiate(L"ACTIVE_UNIFORMS"), f_as(GL_ACTIVE_UNIFORMS));
	a_module->f_put(t_symbol::f_instantiate(L"ACTIVE_UNIFORM_MAX_LENGTH"), f_as(GL_ACTIVE_UNIFORM_MAX_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"ACTIVE_ATTRIBUTES"), f_as(GL_ACTIVE_ATTRIBUTES));
	a_module->f_put(t_symbol::f_instantiate(L"ACTIVE_ATTRIBUTE_MAX_LENGTH"), f_as(GL_ACTIVE_ATTRIBUTE_MAX_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"SHADING_LANGUAGE_VERSION"), f_as(GL_SHADING_LANGUAGE_VERSION));
	a_module->f_put(t_symbol::f_instantiate(L"CURRENT_PROGRAM"), f_as(GL_CURRENT_PROGRAM));
	a_module->f_put(t_symbol::f_instantiate(L"NEVER"), f_as(GL_NEVER));
	a_module->f_put(t_symbol::f_instantiate(L"LESS"), f_as(GL_LESS));
	a_module->f_put(t_symbol::f_instantiate(L"EQUAL"), f_as(GL_EQUAL));
	a_module->f_put(t_symbol::f_instantiate(L"LEQUAL"), f_as(GL_LEQUAL));
	a_module->f_put(t_symbol::f_instantiate(L"GREATER"), f_as(GL_GREATER));
	a_module->f_put(t_symbol::f_instantiate(L"NOTEQUAL"), f_as(GL_NOTEQUAL));
	a_module->f_put(t_symbol::f_instantiate(L"GEQUAL"), f_as(GL_GEQUAL));
	a_module->f_put(t_symbol::f_instantiate(L"ALWAYS"), f_as(GL_ALWAYS));
	a_module->f_put(t_symbol::f_instantiate(L"KEEP"), f_as(GL_KEEP));
	a_module->f_put(t_symbol::f_instantiate(L"REPLACE"), f_as(GL_REPLACE));
	a_module->f_put(t_symbol::f_instantiate(L"INCR"), f_as(GL_INCR));
	a_module->f_put(t_symbol::f_instantiate(L"DECR"), f_as(GL_DECR));
	a_module->f_put(t_symbol::f_instantiate(L"INVERT"), f_as(GL_INVERT));
	a_module->f_put(t_symbol::f_instantiate(L"INCR_WRAP"), f_as(GL_INCR_WRAP));
	a_module->f_put(t_symbol::f_instantiate(L"DECR_WRAP"), f_as(GL_DECR_WRAP));
	a_module->f_put(t_symbol::f_instantiate(L"VENDOR"), f_as(GL_VENDOR));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERER"), f_as(GL_RENDERER));
	a_module->f_put(t_symbol::f_instantiate(L"VERSION"), f_as(GL_VERSION));
	a_module->f_put(t_symbol::f_instantiate(L"EXTENSIONS"), f_as(GL_EXTENSIONS));
	a_module->f_put(t_symbol::f_instantiate(L"NEAREST"), f_as(GL_NEAREST));
	a_module->f_put(t_symbol::f_instantiate(L"LINEAR"), f_as(GL_LINEAR));
	a_module->f_put(t_symbol::f_instantiate(L"NEAREST_MIPMAP_NEAREST"), f_as(GL_NEAREST_MIPMAP_NEAREST));
	a_module->f_put(t_symbol::f_instantiate(L"LINEAR_MIPMAP_NEAREST"), f_as(GL_LINEAR_MIPMAP_NEAREST));
	a_module->f_put(t_symbol::f_instantiate(L"NEAREST_MIPMAP_LINEAR"), f_as(GL_NEAREST_MIPMAP_LINEAR));
	a_module->f_put(t_symbol::f_instantiate(L"LINEAR_MIPMAP_LINEAR"), f_as(GL_LINEAR_MIPMAP_LINEAR));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_MAG_FILTER"), f_as(GL_TEXTURE_MAG_FILTER));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_MIN_FILTER"), f_as(GL_TEXTURE_MIN_FILTER));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_WRAP_S"), f_as(GL_TEXTURE_WRAP_S));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_WRAP_T"), f_as(GL_TEXTURE_WRAP_T));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE"), f_as(GL_TEXTURE));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP"), f_as(GL_TEXTURE_CUBE_MAP));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_BINDING_CUBE_MAP"), f_as(GL_TEXTURE_BINDING_CUBE_MAP));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_POSITIVE_X"), f_as(GL_TEXTURE_CUBE_MAP_POSITIVE_X));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_NEGATIVE_X"), f_as(GL_TEXTURE_CUBE_MAP_NEGATIVE_X));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_POSITIVE_Y"), f_as(GL_TEXTURE_CUBE_MAP_POSITIVE_Y));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_NEGATIVE_Y"), f_as(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_POSITIVE_Z"), f_as(GL_TEXTURE_CUBE_MAP_POSITIVE_Z));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE_CUBE_MAP_NEGATIVE_Z"), f_as(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_CUBE_MAP_TEXTURE_SIZE"), f_as(GL_MAX_CUBE_MAP_TEXTURE_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE0"), f_as(GL_TEXTURE0));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE1"), f_as(GL_TEXTURE1));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE2"), f_as(GL_TEXTURE2));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE3"), f_as(GL_TEXTURE3));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE4"), f_as(GL_TEXTURE4));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE5"), f_as(GL_TEXTURE5));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE6"), f_as(GL_TEXTURE6));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE7"), f_as(GL_TEXTURE7));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE8"), f_as(GL_TEXTURE8));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE9"), f_as(GL_TEXTURE9));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE10"), f_as(GL_TEXTURE10));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE11"), f_as(GL_TEXTURE11));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE12"), f_as(GL_TEXTURE12));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE13"), f_as(GL_TEXTURE13));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE14"), f_as(GL_TEXTURE14));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE15"), f_as(GL_TEXTURE15));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE16"), f_as(GL_TEXTURE16));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE17"), f_as(GL_TEXTURE17));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE18"), f_as(GL_TEXTURE18));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE19"), f_as(GL_TEXTURE19));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE20"), f_as(GL_TEXTURE20));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE21"), f_as(GL_TEXTURE21));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE22"), f_as(GL_TEXTURE22));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE23"), f_as(GL_TEXTURE23));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE24"), f_as(GL_TEXTURE24));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE25"), f_as(GL_TEXTURE25));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE26"), f_as(GL_TEXTURE26));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE27"), f_as(GL_TEXTURE27));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE28"), f_as(GL_TEXTURE28));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE29"), f_as(GL_TEXTURE29));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE30"), f_as(GL_TEXTURE30));
	a_module->f_put(t_symbol::f_instantiate(L"TEXTURE31"), f_as(GL_TEXTURE31));
	a_module->f_put(t_symbol::f_instantiate(L"ACTIVE_TEXTURE"), f_as(GL_ACTIVE_TEXTURE));
	a_module->f_put(t_symbol::f_instantiate(L"REPEAT"), f_as(GL_REPEAT));
	a_module->f_put(t_symbol::f_instantiate(L"CLAMP_TO_EDGE"), f_as(GL_CLAMP_TO_EDGE));
	a_module->f_put(t_symbol::f_instantiate(L"MIRRORED_REPEAT"), f_as(GL_MIRRORED_REPEAT));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_VEC2"), f_as(GL_FLOAT_VEC2));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_VEC3"), f_as(GL_FLOAT_VEC3));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_VEC4"), f_as(GL_FLOAT_VEC4));
	a_module->f_put(t_symbol::f_instantiate(L"INT_VEC2"), f_as(GL_INT_VEC2));
	a_module->f_put(t_symbol::f_instantiate(L"INT_VEC3"), f_as(GL_INT_VEC3));
	a_module->f_put(t_symbol::f_instantiate(L"INT_VEC4"), f_as(GL_INT_VEC4));
	a_module->f_put(t_symbol::f_instantiate(L"BOOL"), f_as(GL_BOOL));
	a_module->f_put(t_symbol::f_instantiate(L"BOOL_VEC2"), f_as(GL_BOOL_VEC2));
	a_module->f_put(t_symbol::f_instantiate(L"BOOL_VEC3"), f_as(GL_BOOL_VEC3));
	a_module->f_put(t_symbol::f_instantiate(L"BOOL_VEC4"), f_as(GL_BOOL_VEC4));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_MAT2"), f_as(GL_FLOAT_MAT2));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_MAT3"), f_as(GL_FLOAT_MAT3));
	a_module->f_put(t_symbol::f_instantiate(L"FLOAT_MAT4"), f_as(GL_FLOAT_MAT4));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLER_2D"), f_as(GL_SAMPLER_2D));
	a_module->f_put(t_symbol::f_instantiate(L"SAMPLER_CUBE"), f_as(GL_SAMPLER_CUBE));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_ENABLED"), f_as(GL_VERTEX_ATTRIB_ARRAY_ENABLED));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_SIZE"), f_as(GL_VERTEX_ATTRIB_ARRAY_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_STRIDE"), f_as(GL_VERTEX_ATTRIB_ARRAY_STRIDE));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_TYPE"), f_as(GL_VERTEX_ATTRIB_ARRAY_TYPE));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_NORMALIZED"), f_as(GL_VERTEX_ATTRIB_ARRAY_NORMALIZED));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_POINTER"), f_as(GL_VERTEX_ATTRIB_ARRAY_POINTER));
	a_module->f_put(t_symbol::f_instantiate(L"VERTEX_ATTRIB_ARRAY_BUFFER_BINDING"), f_as(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING));
	a_module->f_put(t_symbol::f_instantiate(L"IMPLEMENTATION_COLOR_READ_TYPE"), f_as(GL_IMPLEMENTATION_COLOR_READ_TYPE));
	a_module->f_put(t_symbol::f_instantiate(L"IMPLEMENTATION_COLOR_READ_FORMAT"), f_as(GL_IMPLEMENTATION_COLOR_READ_FORMAT));
	a_module->f_put(t_symbol::f_instantiate(L"COMPILE_STATUS"), f_as(GL_COMPILE_STATUS));
	a_module->f_put(t_symbol::f_instantiate(L"INFO_LOG_LENGTH"), f_as(GL_INFO_LOG_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"SHADER_SOURCE_LENGTH"), f_as(GL_SHADER_SOURCE_LENGTH));
	a_module->f_put(t_symbol::f_instantiate(L"SHADER_COMPILER"), f_as(GL_SHADER_COMPILER));
//	a_module->f_put(t_symbol::f_instantiate(L"SHADER_BINARY_FORMATS"), f_as(GL_SHADER_BINARY_FORMATS));
	a_module->f_put(t_symbol::f_instantiate(L"NUM_SHADER_BINARY_FORMATS"), f_as(GL_NUM_SHADER_BINARY_FORMATS));
	a_module->f_put(t_symbol::f_instantiate(L"LOW_FLOAT"), f_as(GL_LOW_FLOAT));
	a_module->f_put(t_symbol::f_instantiate(L"MEDIUM_FLOAT"), f_as(GL_MEDIUM_FLOAT));
	a_module->f_put(t_symbol::f_instantiate(L"HIGH_FLOAT"), f_as(GL_HIGH_FLOAT));
	a_module->f_put(t_symbol::f_instantiate(L"LOW_INT"), f_as(GL_LOW_INT));
	a_module->f_put(t_symbol::f_instantiate(L"MEDIUM_INT"), f_as(GL_MEDIUM_INT));
	a_module->f_put(t_symbol::f_instantiate(L"HIGH_INT"), f_as(GL_HIGH_INT));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER"), f_as(GL_FRAMEBUFFER));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER"), f_as(GL_RENDERBUFFER));
	a_module->f_put(t_symbol::f_instantiate(L"RGBA4"), f_as(GL_RGBA4));
	a_module->f_put(t_symbol::f_instantiate(L"RGB5_A1"), f_as(GL_RGB5_A1));
//	a_module->f_put(t_symbol::f_instantiate(L"RGB565"), f_as(GL_RGB565));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_COMPONENT16"), f_as(GL_DEPTH_COMPONENT16));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_INDEX"), f_as(GL_STENCIL_INDEX));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_INDEX8"), f_as(GL_STENCIL_INDEX8));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_WIDTH"), f_as(GL_RENDERBUFFER_WIDTH));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_HEIGHT"), f_as(GL_RENDERBUFFER_HEIGHT));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_INTERNAL_FORMAT"), f_as(GL_RENDERBUFFER_INTERNAL_FORMAT));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_RED_SIZE"), f_as(GL_RENDERBUFFER_RED_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_GREEN_SIZE"), f_as(GL_RENDERBUFFER_GREEN_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_BLUE_SIZE"), f_as(GL_RENDERBUFFER_BLUE_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_ALPHA_SIZE"), f_as(GL_RENDERBUFFER_ALPHA_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_DEPTH_SIZE"), f_as(GL_RENDERBUFFER_DEPTH_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_STENCIL_SIZE"), f_as(GL_RENDERBUFFER_STENCIL_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE"), f_as(GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_ATTACHMENT_OBJECT_NAME"), f_as(GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL"), f_as(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE"), f_as(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE));
	a_module->f_put(t_symbol::f_instantiate(L"COLOR_ATTACHMENT0"), f_as(GL_COLOR_ATTACHMENT0));
	a_module->f_put(t_symbol::f_instantiate(L"DEPTH_ATTACHMENT"), f_as(GL_DEPTH_ATTACHMENT));
	a_module->f_put(t_symbol::f_instantiate(L"STENCIL_ATTACHMENT"), f_as(GL_STENCIL_ATTACHMENT));
	a_module->f_put(t_symbol::f_instantiate(L"NONE"), f_as(GL_NONE));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_COMPLETE"), f_as(GL_FRAMEBUFFER_COMPLETE));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_INCOMPLETE_ATTACHMENT"), f_as(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"), f_as(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT));
//	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_INCOMPLETE_DIMENSIONS"), f_as(GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_UNSUPPORTED"), f_as(GL_FRAMEBUFFER_UNSUPPORTED));
	a_module->f_put(t_symbol::f_instantiate(L"FRAMEBUFFER_BINDING"), f_as(GL_FRAMEBUFFER_BINDING));
	a_module->f_put(t_symbol::f_instantiate(L"RENDERBUFFER_BINDING"), f_as(GL_RENDERBUFFER_BINDING));
	a_module->f_put(t_symbol::f_instantiate(L"MAX_RENDERBUFFER_SIZE"), f_as(GL_MAX_RENDERBUFFER_SIZE));
	a_module->f_put(t_symbol::f_instantiate(L"INVALID_FRAMEBUFFER_OPERATION"), f_as(GL_INVALID_FRAMEBUFFER_OPERATION));
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

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new gl::xemmai::t_extension(a_module);
}

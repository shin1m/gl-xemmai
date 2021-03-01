add_library(gl MODULE
	error.cc
	buffer.cc
	framebuffer.cc
	renderbuffer.cc
	texture.cc
	program.cc
	shader.cc
	uniform_location.cc
	gl.cc
	)
target_link_libraries(gl xemmai OpenGL::OpenGL)
set_property(TARGET gl PROPERTY PREFIX "")

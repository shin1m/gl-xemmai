xemmaidir = $(XEMMAI_MODULE_PATH)

xemmai_LTLIBRARIES = gl.la
AM_CPPFLAGS = $(GL_CFLAGS) $(XEMMAI_CFLAGS)
AM_CXXFLAGS = -std=c++11
AM_LDFLAGS = -module -avoid-version
if DEBUG
AM_CXXFLAGS += -g
else
AM_CPPFLAGS += -DNDEBUG
AM_CXXFLAGS += -O3
endif
if PROFILE
AM_CXXFLAGS += -pg
AM_LDFLAGS += -pg
endif
gl_la_LIBADD = $(GL_LIBS) $(XEMMAI_LIBS)
gl_la_SOURCES = \
	cast.h \
	array.h \
	error.h \
	error.cc \
	buffer.h \
	buffer.cc \
	framebuffer.h \
	framebuffer.cc \
	renderbuffer.h \
	renderbuffer.cc \
	texture.h \
	texture.cc \
	program.h \
	program.cc \
	shader.h \
	shader.cc \
	uniform_location.h \
	uniform_location.cc \
	gl.h \
	gl.cc

cmake_minimum_required(VERSION 3.11)
project(gl-xemmai VERSION 202504)
find_package(xemmai 202504 REQUIRED)
find_package(OpenGL 1.2 REQUIRED)
add_subdirectory(src)
install(TARGETS gl DESTINATION lib/xemmai-${xemmai_VERSION_MAJOR})

cmake_minimum_required(VERSION 3.24)

project(hypergebra)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -g)

set(SOURCE_FILES ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include)

FILE(GLOB SOURCE_FILES_LIST ${SOURCE_FILES}/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES_LIST})

include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
#PKG_SEARCH_MODULE(GL REQUIRED opengl)
pkg_search_module(GLEW REQUIRED glew)

execute_process(COMMAND python
	${CMAKE_SOURCE_DIR}/build_utils/prepare_shaders.py
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

#[===[
file(STRINGS ${SOURCE_FILES}/shaders/vert.glsl VERT)
file(STRINGS ${SOURCE_FILES}/shaders/frag.glsl FRAG)
file(WRITE ${CMAKE_SOURCE_DIR}/include/shaders.h
"#ifndef SHADERS_H
#define SHADERS_H

#include <string>

const std::string vertex_shader = \"" ${VERT} "\";
const std::string fragment_shader = \"" ${FRAG} "\";

#endif")
]===]

include_directories(${SDL2_INCLUDE_DIRS} ${GL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${GL_LIBRARIES} ${GLEW_LIBRARIES})

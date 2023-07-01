#version 460

#define vert

layout(location = 0) in vec4 position;
layout(location = 4) in vec2 uv_in;

out vec2 uv;

#include "camera.lib"

void main() {
	uv = uv_in;
	gl_Position = position;
}

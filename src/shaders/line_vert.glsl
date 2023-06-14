#version 460

#define vert

layout(location = 0) in vec4 position;
layout(location = 4) in vec2 uv_in;

//uniform float ratio;

out vec2 uv;

void main() {
	gl_Position = position;
	//gl_Position.z = -0.5;
	uv = uv_in;
/*
	if(ratio > 1) {
		uv.x *= ratio;
	} else {
		uv.y /= ratio;
	}
*/
}

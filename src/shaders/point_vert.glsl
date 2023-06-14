#version 460

#define vert

layout(location = 0) in vec4 position;
layout(location = 4) in vec2 uv_in;

uniform float ratio;
uniform float radius;
uniform vec3 coords;

out vec2 uv;

#include "camera.lib"

void main() {
	vec2 pos = worldToClip(coords);
	uv = uv_in;
/*
	if(ratio > 1) {
		uv.x *= ratio;
	} else {
		uv.y /= ratio;
	}
*/
	gl_Position = position;
	//gl_Position.z = 0;
	//gl_Position = vec4((position.xy - pos) * radius + position.xy, position.zw);
	//uv = (uv - pos) * radius + uv;
}

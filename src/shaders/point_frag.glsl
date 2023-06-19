#version 460

#define frag

in vec4 gl_FragCoord;
in vec2 uv;

uniform vec3 color;
uniform float radius;
uniform vec3 coords;


out vec4 frag_color;

#include "camera.lib"

void main() {
	frag_color = vec4(color, 1.0);
	vec2 v = uv - worldToClip(coords);
	float dist = length(v) - radius;

	frag_color.w = 1 - dist / length(vec2(dFdx(dist), dFdy(dist)));
}

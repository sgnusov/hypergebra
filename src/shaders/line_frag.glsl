#version 460

#define frag

in vec4 gl_FragCoord;
in vec2 uv;

uniform vec3 color;
uniform vec3 x0;
uniform vec3 v0;
uniform float width;

out vec4 frag_color;

float d(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y - a.z * b.z;
}

#include "camera.lib"

void main() {
	frag_color = vec4(color, 1.0);

	vec3 p = clipToWorld(uv);

	vec3 x = x0 + v0 * (d(v0, p) / d(-x0, p));
	x /= sqrt(-d(x, x));

	//float dist = acosh(-d(x, p));
	vec2 uv1 = worldToClip(x) - uv;
	float dist = length(uv1) - width / 2;

	frag_color.w = 1 - dist / length(vec2(dFdx(dist), dFdy(dist)));
}

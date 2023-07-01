#version 460

#define frag

in vec4 gl_FragCoord;
in vec2 uv;

uniform bool is_selected;
uniform bool is_hidden;
uniform vec3 color;
uniform vec3 center;
uniform float c;
uniform float width;

out vec4 frag_color;

float d(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y - a.z * b.z;
}

#include "camera.lib"

float getGradientLength(inout float value) {
	float l = length(vec2(dFdx(value), dFdy(value)));
	return clamp(value / l, 0, 1);
}

void main() {
	frag_color = vec4(color, 1.0);
	if(is_hidden)
		frag_color.w = 0.8;
	if(is_selected)
		frag_color.xyz /= 1.2;

	vec3 p = clipToWorld(uv);

	vec3 center1 = center * c;
	vec3 v = p * (-c / (d(p, center))) - center1;
	float mod = sqrt((c * c - 1) / d(v, v));
	vec3 x = center1 + v * mod;

	//float dist = acosh(-d(x, p));
	vec2 uv1 = worldToClip(x) - uv;
	float dist = length(uv1) - width / 2;
	if(dist > 2 * width)
		discard;

	if(is_selected) {
		frag_color = mix(frag_color, vec4(frag_color.xyz, 0.3),  getGradientLength(dist));
		dist -= width;
		if(dist < 0)
			return;
	}
	frag_color.w *= 1 - getGradientLength(dist);
}

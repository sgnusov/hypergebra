#version 460

#define frag

in vec4 gl_FragCoord;
in vec2 uv;

uniform bool is_selected;
uniform vec3 color;
uniform float radius;
uniform vec3 coords;


out vec4 frag_color;

#include "camera.lib"

float getGradientLength(inout float value) {
	float l = length(vec2(dFdx(value), dFdy(value)));
	return clamp(value / l, 0, 1);
}

void main() {
	//float opacity = 0.3;
	//bool is_selected = true;
	//vec3 color_ = color;

	frag_color = vec4(color, 1.0);
	vec2 v = uv - worldToClip(coords);
	float dist = length(v) - radius * 0.8;
	
	if(is_selected) {
		dist -= radius * 1.0;
		frag_color.w = 0.3;
		frag_color = mix(frag_color, vec4(frag_color.xyz, 1.0), getGradientLength(dist));
		dist -= radius * 0.4;
		if(dist < 0)
			return;
		frag_color.w = 1 - getGradientLength(dist);
		return;
	}

	frag_color = mix(frag_color, vec4(vec3(0), 1.0), getGradientLength(dist));
	dist -= radius * 0.2;
	if(dist < 0)
		return;
	frag_color.w = 1 - getGradientLength(dist);
}

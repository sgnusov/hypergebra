uniform mat3 view2world;
uniform mat3 world2view;

float _d(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y - a.z * b.z;
}

float _sq(vec2 a) {
	return a.x * a.x + a.y * a.y;
}

#ifdef frag

vec3 clipToWorld(vec2 uv) {
	if(uv.y <= -1) {
		discard;
	}
	uv = uv - vec2(0, -2);
	uv = 2 * uv / _sq(uv) + vec2(0, -1);
	vec3 p = vec3(uv, 1);
	float mag = -_d(p, p);
	if(mag <= 0) {
		discard;
	}
	mag = 2 / mag;
	return view2world * vec3(p.xy * mag, mag - 1);
}

#endif

vec2 worldToClip(vec3 p) {
	p = world2view * p;
	vec2 uv = p.xy / (p.z + 1);
	uv = uv - vec2(0, -1);
	return 2 * uv / _sq(uv) + vec2(0, -2);
}

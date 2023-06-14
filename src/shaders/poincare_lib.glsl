uniform mat3 view2world;
uniform mat3 world2view;


float _d(vec3 a, vec3 b) {
	return a.x * b.x + a.y * b.y - a.z * b.z;
}

#ifdef frag

vec3 clipToWorld(vec2 uv) {
	vec3 p = vec3(uv, 1);
	float mag = -_d(p, p);
	if(mag < 0) {
		discard;
	}
	mag = 2 / mag;
	return view2world * vec3(p.xy * mag, mag - 1);
}

#endif

vec2 worldToClip(vec3 p) {
	p = world2view * p;
	return p.xy / (p.z + 1);
}

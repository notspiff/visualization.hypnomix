#version 120

/* http://www.lighthouse3d.com/tutorials/glsl-tutorial/directional-lights-i */

attribute vec4 pos;
attribute vec3 nrm;
attribute vec4 clr;

uniform mat4 mvp;
uniform mat3 mnormal;

uniform vec3 lightdir;
uniform vec3 lightclr;
uniform vec3 ambient;

varying vec4 v_clr;

void main() {
//	vec3 n = normalize(mnormal * nrm);
vec3 n = vec3(mvp * vec4(nrm, 0.0)); /* FIXME: work if no scaling! */
	float intensity = max(dot(n, lightdir), 0.0);
	v_clr = vec4(intensity * lightclr + ambient, clr.a);
	gl_Position = mvp * pos;
}

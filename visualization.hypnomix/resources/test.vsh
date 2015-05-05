#version 120

attribute vec4 pos;
attribute vec4 clr;
attribute vec3 nrm;
attribute vec2 texpos;

uniform mat3 mnormal;
uniform mat4 mvp;
uniform mat4 mmodel;

varying vec4 v_color;

void main(void) 
{
	v_color = clr;

	vec4 p = mvp * pos;
	vec4 tmp = normalize(p);
/*
gl_PointSize = tmp.z * 6.0;
	gl_Position = p; */

v_color = vec4(tmp.x, tmp.y, tmp.z, 1.0);
	gl_Position = mvp *pos;
}

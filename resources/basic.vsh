#version 120

#define MAX_LIGHT 2

attribute vec4 pos;
attribute vec4 clr;
attribute vec3 nrm;
attribute vec2 texpos;

uniform mat3 mnormal;
uniform mat4 mvp;
uniform mat4 mmodel;

uniform vec3 lightdir[MAX_LIGHT];
uniform int nblights;

varying vec4 v_color;

void main(void) 
{
	v_color = clr;
	gl_Position = mvp *pos;
}

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
uniform vec3 lightpos[MAX_LIGHT];

varying vec3 normal_direction;
varying vec4 v_color;
varying vec2 v_texpos;
varying vec3 v_lightdir[MAX_LIGHT];
varying vec3 worldpos;

void main(void)
{
	normal_direction = nrm * mnormal;
	v_color = clr;
	int i;
	for(i = 0; i < MAX_LIGHT; i++) {
		v_lightdir[i] = normalize(lightdir[i]);
	}
	v_texpos = texpos;

	vec4 tmp = mmodel * pos;
	worldpos = tmp.xyz;
	gl_Position = mvp * pos;
}

#version 120
#extension GL_EXT_gpu_shader4 : require

#define MAX_LIGHT 2
#define FLAT_SHADING

attribute vec4 pos;
attribute vec4 clr;
attribute vec3 nrm;
attribute vec2 texpos;

uniform mat3 mnormal;
uniform mat4 mvp;
uniform mat4 mmodel;

uniform vec3 lightdir[MAX_LIGHT];
uniform vec3 lightclr[MAX_LIGHT];
uniform int nblights;

flat varying vec4 v_color;


void main(void) 
{
	vec3 normal = normalize(mnormal * nrm);
	vec3 lightdir = lightdir[0];

vec3 ambient = lightclr[0];
	float diffuse = max(0.0, dot(normal, lightdir));
	// float specular = max(0.0, dot(normal, lightdir[0]/2.0));
	float specular = max(0.0, dot(normal, lightdir)); // nstead of lighdir[0], halfvector = lightdir + eyedir...
	float strength = 1.0;

	if(diffuse == 0.0)
		specular = 0.0;	
	else
		specular = pow(specular, 0.5);

	vec3 scattered = ambient + clr.rgb * diffuse;
	vec3 reflected = lightclr[0] * specular * strength;

	v_color = vec4(clr.rgb * scattered + reflected, clr.a);

	gl_Position = mvp * pos;
}

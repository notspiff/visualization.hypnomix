#version 120
#extension GL_EXT_gpu_shader4 : require

#define FLAT_SHADING 1
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
uniform vec3 lightclr[MAX_LIGHT];
uniform vec3 ambient;
uniform float shininess;
uniform float strength;
uniform int nblights;


#ifdef FLAT_SHADING
flat varying vec4 v_color;
#else
varying vec4 v_color;
#endif

/* orange & red book */

void main(void) 
{
	vec4 specular;
	vec3 normal;
	normal = normalize(mnormal * nrm);
	vec3 color = ambient;

	for(int i = 0; i < nblights; i++) {
		float diffuse;
		float specular;
		vec3 scattered, reflected;
		diffuse = max(0.0, dot(normal, lightdir[i]));
		specular = max(0.0, dot(normal, lightpos[i]));
		if(diffuse == 0.0) 
			specular = 0.0;
		else
			specular = pow(specular, shininess);
		scattered = lightclr[i] * diffuse;
		reflected =  lightclr[i] * specular * strength;
		color += min(clr.rgb * scattered + reflected, vec3(1.0));
// color = vec3(lightdir[i].x, 0.0, 0.0);
	}
	
	v_color = vec4(color, clr.a);
	gl_Position = mvp * pos;
}

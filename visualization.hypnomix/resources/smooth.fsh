#version 120

// 
// http://en.wikibooks.org/wiki/GLSL_Programming/GLUT/Specular_Highlights
// http://fr.wikipedia.org/wiki/Ombrage_Phong
// http://devernay.free.fr/cours/opengl/materials.html 
// http://www.ozone3d.net/tutorials/glsl_lighting_phong.php
//

#define MAX_LIGHT 2

varying vec4 v_color;
varying vec3 normal_direction;
varying vec2 v_texpos;
varying vec3 v_lightdir[MAX_LIGHT];
varying vec3 worldpos;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec3 lightpos[MAX_LIGHT];
uniform vec4 lightclr[MAX_LIGHT];
uniform int nblights;


struct lightSource {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 direction;
};

lightSource light0 = lightSource(
	vec4(0.01, 0.01, 0.01, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec3(0.0, 1.0, -1.0)
);

struct material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;	
	float shininess;
};

material mat0 = material( // chrome
	vec4(0.25, 0.25, 0.25, 1.0),
	vec4(0.4, 0.4, 0.4, 1.0),
	vec4(0.774597, 0.774597, 0.774597, 1.0),
	0.6 * 128.0 
);

material mat1 = material(
	vec4(0.2125, 0.1275, 0.054, 1.0),
	vec4(0.714, 0.4284, 0.18144, 1.0),
	vec4(0.393548, 0.271906, 0.166721, 1.0),
	25.6
);

/* http://www.opengl.org/discussion_boards/showthread.php/132502-Color-tables */
vec4 bronze = vec4(0.55, 0.47, 0.14, 1.0);
vec4 silver = vec4(0.90, 0.91, 0.98, 1.0);
vec4 oldgold = vec4(0.81, 0.71, 0.23, 1.0);


vec4 lightCompute(vec3 lightdir, vec4 lightclr)
{
	float lightcos = dot(normal_direction, -lightdir);

	// ambient
//	vec4 texture = (texture2D(texture0, v_texpos) + texture2D(texture1, v_texpos)) * 0.5;
//	vec4 iambient = lightclr * mat0.ambient 
//		+ texture * v_color * silver * mat0.ambient;
	vec4 iambient = lightclr * mat0.ambient;

	// diffuse
	float lambert = max(0.0, lightcos);
	vec4 idiffuse = light0.diffuse * lambert * mat0.diffuse;

	// specular
	vec3 reflect = reflect(normal_direction, -lightdir);
	float specular = pow(max(dot(reflect, lightdir), 0.0), mat0.shininess);
	vec4 ispecular = mat0.specular * light0.specular * specular;

	return(iambient + idiffuse + ispecular);
}


void main(void) 
{
	int i;
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

	// spot lights
	for(i = 0; i < nblights; i++) {
//		color += lightCompute(v_lightdir[i], lightclr[i]);
	}

	float attconst = 1.0;
	float attlinear = 0.045;
	float attexp = 0.0075;

	for(i = 0; i < nblights; i++) {
		vec3 lightdir = worldpos - lightpos[i];
		float dist = length(lightdir);
		lightdir = normalize(lightdir);
		vec4 colorpoint = lightCompute(lightdir, lightclr[i]);

		float att = 1.0 / (attconst + attlinear * dist + attexp * dist * dist);

		color += colorpoint * att;
		// color = vec4(lightdir, 0.5);
		// color = lightclr[i];
	}

	gl_FragColor = color;
//	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

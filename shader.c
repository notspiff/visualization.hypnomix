#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "shader.h"


char *shaderReadFile(const char *filename) 
{
	FILE *fp = fopen(filename, "r");
	char *buf;
	struct stat stat_info;

	if(fp == NULL) {
		fprintf(stderr, "ERROR: shaderReadFile(): %s does not exist\n",
			 filename);
		return NULL;
	}

	stat(filename, &stat_info);
	buf = (char*)malloc(stat_info.st_size + 1);
	if(buf == NULL) {
		fprintf(stderr, "ERROR: shaderReadFile(): buf is NULL\n");
		return NULL;
	}
	if(fread(buf, 1, stat_info.st_size, fp) < stat_info.st_size) {
		return NULL;
	}
	buf[stat_info.st_size] = '\0';
	fclose(fp);

fprintf(stderr, "SHADER: read %s\n", filename);

	return buf;
}


static int shaderCompile(GLuint prg, GLuint shader, const char *src)
{
	int len;
	GLint status;
	char msg[256];

	if(src == NULL) {
		printf("ERROR: shaderCompile(): source is NULL\n"); 
		return -1;
	}
	len = strlen(src);
	glShaderSource(shader, 1, &src, &len);
	glCompileShader(shader);	
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(!status) {
		glGetShaderInfoLog(shader, sizeof(msg), 0, msg);
		printf("ERROR: shaderCompile(): shader %s: %s\n", 
			src, msg);
		return -1;
	} else {
		printf("SUCCESS: shaderCompile():\n%s\n", src);
	}
	glAttachShader(prg, shader);

	return 0;
}


int shaderInit(GLuint *program, const char *vtxsrc, const char *frgsrc,
	const char *geosrc) 
{
	char msg[256];
	GLint status;
	GLuint vertex, fragment, geometry; 
	int maxv; 

	(*program) = glCreateProgram();
	if(program == 0) {
		fprintf(stderr, "ERROR: shaderInit(): create program\n");
		return -1;
	}

	// compile vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	shaderCompile((*program), vertex, vtxsrc);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	shaderCompile((*program), fragment, frgsrc);
	if(geosrc != NULL) {
		glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &maxv);
fprintf(stderr, "MAX_GEOMETRY_VERTICES: %d\n", maxv);

		glProgramParameteriEXT((*program), 
			GL_GEOMETRY_VERTICES_OUT_EXT, maxv);
		glGetProgramiv((*program), GL_GEOMETRY_VERTICES_OUT_EXT, &maxv);
fprintf(stderr, "GEOMETRY_VERTICES: %d\n", maxv);
		geometry = glCreateShader(GL_GEOMETRY_SHADER_EXT);
		shaderCompile((*program), geometry, geosrc);
	}
	
	// link
	glLinkProgram((*program));

	glGetProgramiv((*program), GL_LINK_STATUS, &status);
	if(!status) {
		glGetProgramInfoLog((*program), sizeof(msg), 0, msg);
		printf("ERROR: shaderInit(): link shader program: %s\n", msg);
		return -1;
	} else {
		printf("SUCCESS: shaderInit(): link shader program\n");
	}

	// une fois le shader associe au programme on peut liberer la memoire
	if(vertex) {
		glDetachShader((*program), vertex);
		glDeleteShader(vertex);
	}

	if(fragment) {
		glDetachShader((*program), fragment);
		glDeleteShader(fragment);
	}
	if(geometry) {
		glDetachShader((*program), geometry);
		glDeleteShader(geometry);
	}
	return 0;
}


int shaderFileInit(GLuint *program, const char *vtxfilename, 
	const char *frgfilename, const char *geofilename) 
{
	int err;
	const GLchar *vtxsrc = shaderReadFile(vtxfilename);
	const GLchar *frgsrc = shaderReadFile(frgfilename);
	const GLchar *geosrc = NULL;

	if(geofilename != NULL) {
		geosrc = shaderReadFile(geofilename);
	} 


	if(vtxsrc == NULL || frgsrc == NULL) {
		return -1;
	}
	err = shaderInit(program, vtxsrc, frgsrc, geosrc);
	free((char*)geosrc);
	free((char*)frgsrc);
	free((char*)vtxsrc);

	return err;
}


GLuint shaderGetUniform(GLuint prog, const char *name)
{
	GLuint loc;

	loc = glGetUniformLocation(prog, name);
	if(loc == -1) {
		printf("WARNING: shaderGetUniform(): uniform %s not found in shader\n", name);
	}

	return loc;
}


GLuint shaderGetAttribute(GLuint prog, const char *name)
{
	GLuint loc;

	loc = glGetAttribLocation(prog, name);
	if(loc == -1) {
		printf("WARNING: shaderGetAttribute(): attribute %s not found in shader\n", name);
	}

	return loc;
}


void shaderDestroy(GLuint prg)
{
        glDeleteProgram(prg);
}


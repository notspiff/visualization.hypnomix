#ifndef __SHADER_H__
#define __SHADER_H__


#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#endif


typedef struct program {
	GLuint id;
	GLuint mvp;
	GLuint mnormal;
	GLuint mmodel;
	GLuint pos;
	GLuint nrm;
	GLuint clr;
	GLuint texpos;
	GLuint texture0;
	GLuint texture1;
	GLuint lightpos;
	GLuint lightdir;
	GLuint lightclr;
	GLuint ambient;
	GLuint shininess;
	GLuint strength;
	GLuint nblights;
} program_t;


char *shaderReadFile(const char *filename);
int shaderInit(GLuint *program, const char *vtxsrc, const char *frgsrc, const char *geosrc);
int shaderFileInit(GLuint *program, const char *vtxfilname, const char *frgfilename, const char *geofilename);
GLuint shaderGetUniform(GLuint prog, const char *name);
GLuint shaderGetAttribute(GLuint prog, const char *name);
void shaderDestroy(GLuint prg);


#endif

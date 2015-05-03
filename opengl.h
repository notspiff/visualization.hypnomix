#ifndef __OPENGL_H__
#define __OPENGL_H__


#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif


struct point2D {
	float x;
	float y;
	float s;
	float t;
} point2D_t;


struct point3D {
	float x;
	float y;
	float z;
	float s;
	float t;
} point3D_t;


void setVtx(float *vtx, float x, float y, float z);
void setClr(float *clr, float r, float g, float b, float a);


#endif

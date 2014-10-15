#ifndef __FONT_H__
#define __FONT_H__


#include "opengl.h"
#include "hypnomix.h"

/*
GLuint fontprg;
GLuint fontpos;
GLuint fonttexpos;
GLuint fonttexture;
GLuint fontmvp;
*/

int fontInit(struct hypnomix *hyp, const char* ftname, int w, int h);
void fontDestroy(void);
void fontPrint(struct hypnomix *hyp, const char *txt, 
	float x, float y, int r, int g, int b, int a);
void fontDrawChar(unsigned char *buf, float x, float y, float w, float h);
void fontDrawFramebuffer(struct hypnomix *hyp);


#endif

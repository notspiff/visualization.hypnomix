#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

/*
enum {
	TEXTURE_FONT,
	TEXTURE_PEONS,
	TEXTURE_TILES,
	TEXTURE_CONSOLE,
	TEXTURE_METAL,
	TEXTURE_FANART,
	TEXTURE_PARTICLE,
	TEXTURE_FUN,
	TEXTURE_FREETYPE,
	TEXTURE_FREETYPE_RENDER,
	TEXTURE_NUM
};


struct texture {
	char *imagename;
	GLuint id;
	float w;
	float h;
} texture_t;


GLuint textures[TEXTURE_NUM];

*/


void textureInit(GLuint *textures, int nbtex);
void textureLoadWithFilename(const char *filename, GLuint *texturename);
void textureDestroy(GLuint *textures, int nbtex);


#endif 

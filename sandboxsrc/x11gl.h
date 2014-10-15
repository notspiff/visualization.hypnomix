#ifndef __X11GL_H__
#define __X11GL_H__

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "x11keycode.h"


int x11glquit;

void X11GLInit(int x, int y, int w, int h);
void X11GLDestroy(void);
void X11GLEvent(void);
void X11RegisterKeypress(void (*f)(unsigned int keycode));
void X11RegisterMouse(void (*move)(int, int),
	void (*press)(unsigned int, int, int), 
	void (*release)(unsigned int, int, int));
void X11GLSwapBuffers(void);


#endif

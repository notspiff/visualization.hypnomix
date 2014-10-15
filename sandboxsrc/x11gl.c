#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include "x11gl.h"


/* FIXME add destroy notify command when user close the window */


Display *dpy;
Window win;
XVisualInfo *vi;
GLXContext glc;


static void keypressFoo(unsigned int key) {}
static void mousemoveFoo(int x, int y) {}
static void buttonFoo(unsigned int but, int x, int y) {} 


static void (*keypress)(unsigned int key) = keypressFoo;
static void (*mousemove)(int x, int y) = mousemoveFoo;
static void (*buttonpress)(unsigned int but, int x, int y) = buttonFoo;
static void (*buttonrelease)(unsigned int but, int x, int y) = buttonFoo;


void X11GLInit(int x, int y, int w, int h)
{
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	Window root;
	Colormap cmap;
	XSetWindowAttributes swa;
	int minor, major;

	dpy = XOpenDisplay(getenv("DISPLAY"));
	if(dpy == NULL) {
		fprintf(stderr, "[ERROR] X11GLInit(): XOpenDisplay()\n");
		exit(0);
	}

	root = DefaultRootWindow(dpy);

	vi = glXChooseVisual(dpy, 0, att);
	if(vi == NULL) {
		fprintf(stderr, "[ERROR] X11GLInit(): glXChooseVisual()\n");
		exit(0);
	}

	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

	swa.colormap = cmap;
	swa.event_mask = ExposureMask|KeyPressMask
		|ButtonPressMask|ButtonReleaseMask|PointerMotionMask;

	win = XCreateWindow(dpy, root, x, y, w, h, 0, vi->depth, 
		InputOutput, vi->visual, CWColormap|CWEventMask, &swa);

	XMapWindow(dpy, win);
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);

	glXQueryVersion(dpy, &major, &minor);

	XFree(vi);

	x11glquit = 0;

	fprintf(stderr, "[SUCCESS] X11GLInit() - GLX %d.%d\n", major, minor);
}


void X11GLDestroy(void)
{
	glXMakeCurrent(dpy, None, NULL);
	glXDestroyContext(dpy, glc);
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}


void X11GLEvent(void)
{
	XEvent xev;
	char buf[256];
	KeySym keysym;
	int nbytes;
	KeyCode keycode;

	while(XPending(dpy)) {
		XNextEvent(dpy, &xev);

		switch(xev.type) {
			case Expose:
				glXSwapBuffers(dpy, win);
				break;
			case KeyPress:
				nbytes =  XLookupString(&xev.xkey, buf, 255, 
					&keysym, NULL);
				keycode = XKeysymToKeycode(dpy, keysym);
				(*keypress)(keycode);
fprintf(stderr, "keycode: %u\n", keycode);
				break;
			case ButtonPress:
				(*buttonpress)(xev.xbutton.button,
					xev.xbutton.x, xev.xbutton.y);
				break;
			case ButtonRelease:
				(*buttonrelease)(xev.xbutton.button,
					xev.xbutton.x, xev.xbutton.y);
				break;
			case MotionNotify:
				(*mousemove)(xev.xmotion.x, xev.xmotion.y);
				break;
			default:
				break;
		}
	}
}


void X11RegisterKeypress(void (*f)(unsigned int keycode))
{
	keypress = f;
}


void X11RegisterMouse(void (*move)(int, int),
	void (*press)(unsigned int, int, int), 
	void (*release)(unsigned int, int, int))
{
	mousemove = move;
	buttonpress = press;
	buttonrelease = release;
}
	


void X11GLSwapBuffers(void)
{
	glXSwapBuffers(dpy, win);
}

#include "opengl.h"


void setVtx(float *vtx, float x, float y, float z)
{
	vtx[0] = x;
	vtx[1] = y;
	vtx[2] = z;
}


void setClr(float *clr, float r, float g, float b, float a)
{
	clr[0] = r;
	clr[1] = g;
	clr[2] = b;
	clr[3] = a;
}

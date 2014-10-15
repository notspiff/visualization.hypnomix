#include <stdio.h>

#include "hypnomix.h"
#include "pool.h"


#define MAX_OBJECTS 256 /* FIXME: make this dynamic */
#define MAGICNB 0.7071067814 /* 1 / sqrt(2), https://en.wikipedia.org/wiki/Tetrahedron */


static struct pool povert;
static struct pool poidx;


char modname[] = "7rain";


float tetrahedronvtx[] = {
	1.0, 0.0, -MAGICNB,
	-1.0, 0.0, -MAGICNB,
	0.0, 1.0, MAGICNB,
	0.0, -1.0, MAGICNB
};
unsigned int tetrahedronidx[] = {
	0, 1, 2,
	1, 3, 2,
	0, 2, 3,
	0, 3, 1
};
float tetrahedronclr[] = {
	1.0, 0.0, 0.0, 1.0,
	0.0, 1.0, 0.0, 1.0,
	0.0, 0.0, 1.0, 1.0,
	1.0, 0.0, 1.0, 1.0
};


struct onevertice {
	float x;
	float y;
	float z;
	float nrmx;
	float nrmy;
	float nrmz;
	float r;
	float g;
	float b;
	float a;
} onevertice_t;


// struct onevertice vertices[MAX_OBJECTS*12]; /* 12 vtx for the tetrahedron */
// unsigned int idx[MAX_OBJECTS*3*12];


static void addObject(float scale, float x, float y, float z, float r, float g, float b, float a)
{
	int i;
	struct onevertice v;
	unsigned int idx;
	unsigned int nxtid;

	for(i = 0; i < 4; i++) {
		v.x = tetrahedronvtx[i*3] * scale + x;
		v.y = tetrahedronvtx[i*3+1] * scale + y;
		v.z = tetrahedronvtx[i*3+2] * scale + z;
		v.nrmx = 0.0;
		v.nrmy = 0.0;
		v.nrmz = 0.0;
		v.r = r; //tetrahedronclr[i*4];
		v.g = g; //tetrahedronclr[i*4+1];
		v.b = b; //tetrahedronclr[i*4+2];
		v.a = a; //tetrahedronclr[i*4+3];
		poolAdd(&povert, &v);
	}

	nxtid = poidx.id / 3;
	for(i = 0; i < 12; i++) {
		idx = tetrahedronidx[i]+nxtid;
		poolAdd(&poidx, &idx);
	}
/*
unsigned int *t;
fprintf(stderr, "POIDX.ID=%d)", poidx.id);
for(i = 0; i < poidx.id; i++) {
	t = (unsigned int*)(poidx.pool+i*poidx.size);
	fprintf(stderr, "%d ", *t);
} */
}


void init(struct hypnomix *hyp)
{
	transformProjection(&hyp->tr, hyp->pmatrix);
	hyp->var.nbbands = 3;

	poolInit(&povert, MAX_OBJECTS*4, sizeof(struct onevertice));
	poolInit(&poidx, MAX_OBJECTS*12, sizeof(unsigned int)); /* one object is 12 idx */

int i, j;
for(i = 0; i < 16; i++) {
	for(j = 0; j < 16; j++) {
		addObject(0.2, -1.0*0.5*j, -1.0+0.5*i, 0.0, i/10.0, j/10.0, 1.0, 1.0);
fprintf(stderr, "%d ", poidx.id);
	}
}
}


void destroy(struct hypnomix *hyp)
{
	poolDestroy(&poidx);
	poolDestroy(&povert);
}


static float angle = 0.0;


void draw(struct hypnomix *hyp)
{
	unsigned int stride = sizeof(struct onevertice);

	transformIdentity(&hyp->tr);
	transformModelTranslate(&hyp->tr, 0.0, 0.0, -6.0);
	transformModelRotate(&hyp->tr, angle, 0.0, 0.0);
	transformMVP(&hyp->tr);
	angle += 0.01;
/*
struct onevertice *v;
int i;
for(i = 0; i < 4; i++) {
v = vertices + i;
printf("%f %f %f\n", v->x, v->y, v->z);
} */

	glUseProgram(hyp->pg.id);
	glUniformMatrix4fv(hyp->pg.mvp, 1, GL_FALSE, hyp->tr.mvp);
	glUniformMatrix3fv(hyp->pg.mnormal, 1, GL_FALSE, hyp->tr.mnormal);
//	glUniformMatrix4fv(mod7rain.pg.mmodel, 1, GL_FALSE, mmodel);


	glEnableVertexAttribArray(hyp->pg.pos);
	glEnableVertexAttribArray(hyp->pg.clr);
	glEnableVertexAttribArray(hyp->pg.nrm);
//		glUniform3fv(hyp->pg.lightdir, hyp->lights.nb, hyp->lights.dir);
	glUniform3fv(hyp->pg.lightclr, hyp->lights.nb, hyp->lights.clr);
	glUniform1i(hyp->pg.nblights, hyp->lights.nb);

//

	glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, stride, 
		(GLvoid *)povert.pool);
	glVertexAttribPointer(hyp->pg.nrm, 3, GL_FLOAT, GL_FALSE, stride, 
		(GLvoid *)(povert.pool)+6*sizeof(float));
	glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, stride, 
		(GLvoid *)(povert.pool)+6*sizeof(float));

	// glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, idx);
/*
int i;
unsigned int *a;
unsigned int test[12];
fprintf(stderr, "POIDX.ID=%d)", poidx.id);
for(i = 0; i < poidx.id; i++) {
	a = (unsigned int*)(poidx.pool+i*poidx.size);
	fprintf(stderr, "(%d) ", *a);
	test[i] = *a;
}
a = (unsigned int *)poidx.pool;
fprintf(stderr, "<%d> ", *a);
fprintf(stderr, "<%d> ", *(a+1)); */
	// glDrawElements(GL_TRIANGLES, poidx.id, GL_UNSIGNED_INT, (unsigned int *)poidx.pool);
	glDrawElements(GL_TRIANGLES, poidx.id, GL_UNSIGNED_INT, (GLvoid *)poidx.pool);

	glDisableVertexAttribArray(hyp->pg.nrm);
	glDisableVertexAttribArray(hyp->pg.clr);
	glDisableVertexAttribArray(hyp->pg.pos);
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "opengl.h"
#include "hypnomix.h"
#include "math.h"


char modname[] = "geography";

#define GOLDEN_RATIO 1.61803398874989484820
#define ISOCAHEDRON_MAX_VERTICES 12 
#define ISOCAHEDRON_MAX_FACES 20

/* use this macro to float comparison : a == b does not work */
#define FLOATCMP(a, b) (fabs(a - b) < 0.000001)


static GLfloat vertices[] = { /* http://pastebin.com/WSLwiZnq */
	GOLDEN_RATIO, 1.0f, 0.0f,
	GOLDEN_RATIO, -1.0f, 0.0f,
	1.0f, 0.0f, GOLDEN_RATIO,
	1.0f, 0.0f, -GOLDEN_RATIO,
	0.0f, GOLDEN_RATIO, 1.0f,
	0.0f, GOLDEN_RATIO, -1.0f,
	0.0f, -GOLDEN_RATIO, 1.0f,
	0.0f, -GOLDEN_RATIO, -1.0f,
	-1.0f, 0.0f, GOLDEN_RATIO,
	-1.0f, 0.0f, -GOLDEN_RATIO,
	-GOLDEN_RATIO, 1.0f, 0.0f,
	-GOLDEN_RATIO, -1.0f, 0.0f 
};

static GLuint indices[] = {
	0, 5, 4,
	0, 4, 2,
	0, 2, 1,
	0, 1, 3,
	0, 3, 5,
	1, 6, 7,
	1, 7, 3,
	1, 2, 6,
	2, 8, 6,
	2, 4, 8,
	3, 7, 9,
	3, 9, 5,
	4, 10, 8,
	4, 5, 10,
	5, 9, 10,
	6, 11, 7,
	6, 8, 11,
	7, 11, 9,
	8, 10, 11,
	9, 11, 10	
};

#define MAX_SUBDIVISION 4
#define MAX_FACES 5120 * 8 /* 20 * 4^4 -> 4 faces per triangle per level * 8 faces per bulb */
#define MAX_VERTICES MAX_FACES * 3

static int levels = 0; /* number of subdivision */

static int nbvtx;
static GLfloat vtx[MAX_VERTICES * 3];
static GLfloat morphvtx[MAX_VERTICES * 3];
static GLfloat nrm[MAX_VERTICES * 3]; /* one normal per vertice */
static int nbidx;
static GLuint idx[MAX_FACES * 3];
static GLfloat clr[MAX_VERTICES * 4];

/*
static void setVtx(float *vtx, float x, float y, float z)
{
	vtx[0] = x;
	vtx[1] = y;
	vtx[2] = z;
}


static void setClr(float *clr, float r, float g, float b, float a)
{
	clr[0] = r;
	clr[1] = g;
	clr[2] = b;
	clr[3] = a;
} */


static void lineMiddle(struct point3D *a, struct point3D *b, struct point3D *m)
{
	m->x = (b->x + a->x) / 2.0;
	m->y = (b->y + a->y) / 2.0;
	m->z = (b->z + a->z) / 2.0;
}


static void addVertex(GLfloat *vtx, GLuint *idx, int *nbvtx, int nbidx,
	struct point3D *v) 
{
	float len;

	len = vec3Mag((GLfloat *)v);
	if(len > 0) {
		v->x /= len;
		v->y /= len;
		v->z /= len;
		idx[nbidx] = (*nbvtx);
		setVtx(vtx+(*nbvtx)*3, v->x, v->y, v->z);
		(*nbvtx)++;
	}
}


static void addTriangle(GLfloat *vtx, GLuint *idx, int *nbvtx, int *nbidx,
	struct point3D *a, struct point3D *b, struct point3D *c)
{
	addVertex(vtx, idx, nbvtx, (*nbidx) * 3, a);
	addVertex(vtx, idx, nbvtx, (*nbidx) * 3 + 1, b);
	addVertex(vtx, idx, nbvtx, (*nbidx) * 3 + 2, c);

	(*nbidx)++; /* next face */
}


static void addBulb(GLuint a, GLuint b, GLuint c)
{
	idx[nbidx*3] = a;
	idx[nbidx*3+1] = b;
	idx[nbidx*3+2] = c;
	nbidx++; 
}


void isocahedronBulb(void)
{
	int i;
	GLuint v1, v2, v3;
	GLuint va, vb, vc;
	GLuint oldnbidx = nbidx;

	for(i=0; i < oldnbidx; i++) {
		v1 = idx[i*3];
		v2 = idx[i*3+1];
		v3 = idx[i*3+2];

		va = nbvtx++;
		setVtx(&vtx[va*3], vtx[v1*3], vtx[v1*3+1], vtx[v1*3+2]);
		setClr(&clr[va*4], 
			clr[v1*4], clr[v1*4+1], clr[v1*4+2], clr[v1*4+3]);

		vb = nbvtx++;
		setVtx(&vtx[vb*3], vtx[v2*3], vtx[v2*3+1], vtx[v2*3+2]);
		setClr(&clr[vb*4], 
			clr[v2*4], clr[v2*4+1], clr[v2*4+2], clr[v2*4+3]);

		vc = nbvtx++;
		setVtx(&vtx[vc*3], vtx[v3*3], vtx[v3*3+1], vtx[v3*3+2]);
		setClr(&clr[vc*4], 
			clr[v3*4], clr[v3*4+1], clr[v3*4+2], clr[v3*4+3]);
// #define ONLY_FACES
#ifndef ONLY_FACES
		addBulb(va, vb, v1);
		addBulb(v1, vb, v2);
		addBulb(v2, vb, v3);
		addBulb(v3, vb, vc);
		addBulb(v3, vc, v1);
		addBulb(v1, vc, va);
/*
		addBulb(v1, va, v3);
		addBulb(va, vc, v3);
		addBulb(v2, vb, v1);
		addBulb(vb, va, v1);
		addBulb(v3, vc, v2);
		addBulb(v2, vc, vb); */
#endif
	}

	printf("nbvtx=%d, nbidx=%d\n", nbvtx, nbidx);
}


void isocahedronMorph(const float *data, int nbdata)
{
	int i, j;
	GLuint v1, v2, v3;
	GLfloat mulx, muly, mulz;
	int nbmorphidx = 20 * pow(4, levels);
	GLfloat *a, *b, *c;
	vec3 vnormal;
	
//	i = startidx - 1;
i = 0;
	j = 0;
//	while(i < nbidx) { /* FIXME: do something! :-D */
while(i < nbmorphidx) {
		v1 = idx[i*3];
		v2 = idx[i*3+1];
		v3 = idx[i*3+2];

		// mulx = muly = mulz = fabs(cos(i+angle)) * 0.3;	
		mulx = muly = mulz = fabs(data[j]) * 1.2;	
// mulx = muly = mulz = 1.0;

		if(++j >= nbdata) {/* little check */
			j = 0;	
		}

		a = vtx + v1 * 3;
		b = vtx + v2 * 3;
		c = vtx + v3 * 3;
		vec3Normal(vnormal, a, b, c);

		vnormal[0] *= mulx;
		vnormal[1] *= muly;
		vnormal[2] *= mulz;

		morphvtx[v1*3] = vtx[v1*3] + vnormal[0];
		morphvtx[v1*3+1] = vtx[v1*3+1] + vnormal[1];
		morphvtx[v1*3+2] = vtx[v1*3+2] + vnormal[2];

		morphvtx[v2*3] = vtx[v2*3] + vnormal[0]; 
		morphvtx[v2*3+1] = vtx[v2*3+1] + vnormal[1];
		morphvtx[v2*3+2] = vtx[v2*3+2] + vnormal[2];

		morphvtx[v3*3] = vtx[v3*3] + vnormal[0];
		morphvtx[v3*3+1] = vtx[v3*3+1] + vnormal[1];
		morphvtx[v3*3+2] = vtx[v3*3+2] + vnormal[2];

//	 	i += 7; /* next extern face */
i++;
	}
	transformComputeAverageNormals(idx, nbidx, vtx, nbvtx, nrm, 0);
}


float iso_line = 0.0;


void init(struct hypnomix *hyp)
{
	int nbsubs = 2;
// int nbsubs = 1;
	int face;
	int i;
	int sub;
	GLuint v1, v2, v3;
	struct point3D a, b, c;
	struct point3D mab, mac, mbc;
	GLfloat *vtxtmp;
	GLuint *idxtmp;
	int nbvtxtmp, nbidxtmp;
	float red, green, blue, alpha;

	levels = nbsubs;

	transformProjection(&hyp->tr, hyp->pmatrix);
	hyp->var.nbbands = 3;

fprintf(stderr, "\n\nGEOGRAPHY INIT\n\n");

	if(nbsubs > MAX_SUBDIVISION)
		nbsubs = MAX_SUBDIVISION;

	memcpy(vtx, vertices, sizeof(vertices));
	memcpy(idx, indices, sizeof(indices));
	nbvtx = 12;
	nbidx = 20;

	for(sub = 0; sub < nbsubs; sub++) {
		printf("sub(%d)\n", sub);

		vtxtmp = (GLfloat *)malloc(MAX_VERTICES * 3 * sizeof(GLfloat));
		assert(vtxtmp != NULL);
		idxtmp = (GLuint *)malloc(MAX_FACES * 3 * sizeof(GLuint));
		assert(idxtmp != NULL);

		bzero(vtxtmp, MAX_VERTICES * 3 * sizeof(GLfloat));
		bzero(idxtmp, MAX_FACES * 3 * sizeof(GLfloat));

		nbvtxtmp = 0;
		nbidxtmp = 0;

		face = 0;
		while(face < nbidx) {
		// while(face < 1) {
			v1 = idx[face*3];
			v2 = idx[face*3+1];
			v3 = idx[face*3+2];

			a.x = vtx[v1*3];
			a.y = vtx[v1*3+1];
			a.z = vtx[v1*3+2];

			b.x = vtx[v2*3];
			b.y = vtx[v2*3+1];
			b.z = vtx[v2*3+2];

			c.x = vtx[v3*3];
			c.y = vtx[v3*3+1];
			c.z = vtx[v3*3+2];

			lineMiddle(&a, &b, &mab);
			lineMiddle(&a, &c, &mac);
			lineMiddle(&b, &c, &mbc);
			
			addTriangle(vtxtmp, idxtmp, &nbvtxtmp, &nbidxtmp, 
				&a, &mab, &mac);
			addTriangle(vtxtmp, idxtmp, &nbvtxtmp, &nbidxtmp, 
				&mab, &b, &mbc);
			addTriangle(vtxtmp, idxtmp, &nbvtxtmp, &nbidxtmp, 
 				&mac, &mbc, &c);
			addTriangle(vtxtmp, idxtmp, &nbvtxtmp, &nbidxtmp, 
				&mab, &mbc, &mac);

			face++;
		}

printf("nbvtxtmp=%d, nbidxtmp=%d\n", nbvtxtmp, nbidxtmp);

		memcpy(vtx, vtxtmp, nbvtxtmp * sizeof(GLfloat) * 3);
		memcpy(idx, idxtmp, nbidxtmp * sizeof(GLuint) * 3);

		nbvtx = nbvtxtmp;
		nbidx = nbidxtmp;

		free(vtxtmp);
		free(idxtmp);

	}

	srand(time(NULL));
	/* set color to random gradient */
	red = hyp->pr.clr0[0];
	green = hyp->pr.clr0[1];
	blue = hyp->pr.clr0[2];
	alpha = hyp->pr.clr0[3];
	for(i = 0; i < nbidx; i++) {
		setClr(clr+idx[i*3]*4, red, green, blue, alpha);
		setClr(clr+idx[i*3+1]*4, red, green, blue, alpha);
		setClr(clr+idx[i*3+2]*4, red, green, blue, alpha);
	}

	isocahedronBulb();
	transformComputeAverageNormals(idx, nbidx, vtx, nbvtx, nrm, 0);
	memcpy(morphvtx, vtx, nbvtx * sizeof(GLfloat) * 3);
}


void destroy(struct hypnomix *hyp)
{
	/* nothing to do */
fprintf(stderr, "\n\nGEOGRAPHY DESTROY\n\n");
}


void draw(struct hypnomix *hyp)
{
	float ax, ay;
vec3 lightpos, lightdir;
/*
static float ax = 0.0;
static float ay = 0.0;
ay += 0.01;
*/
	isocahedronMorph(hyp->morphsamples, hyp->nbsamples);

	ax = hyp->var.smooth[0] * hyp->pr.var[0] * M_PI;
	ay = hyp->var.smooth[1] * hyp->pr.var[1] * M_PI;

	//az = hyp->var.smoothh * M_PI;

	transformIdentity(&hyp->tr);
	//transformModelTranslate(&hyp->tr, 0.0, 0.0, -6.0);
	transformModelRotate(&hyp->tr, ax, ay, 0.0);
	// transformViewRotate(&hyp->tr, 0.0, ay, 0.0);
	transformViewTranslate(&hyp->tr, 0.0, 0.0, -8.0);
	transformMVP(&hyp->tr);

	glUseProgram(hyp->pg.id);
	glEnableVertexAttribArray(hyp->pg.pos);
	glEnableVertexAttribArray(hyp->pg.clr);
	glEnableVertexAttribArray(hyp->pg.nrm);
/*
	transformMVCoordinates(&hyp->tr, &(hyp->lights.dir[0]), lightdir);
	transformMVCoordinates(&hyp->tr, &(hyp->lights.pos[0]), lightpos);
	glUniform3fv(hyp->pg.lightdir, hyp->lights.nb, lightdir);
	glUniform3fv(hyp->pg.lightdir, hyp->lights.nb, lightdir);
*/

	transformToView(&hyp->tr, hyp->lights.dir[0], lightpos);
	glUniform3fv(hyp->pg.lightdir, hyp->lights.nb, lightpos);

	if(hyp->pg.lightclr != -1) {
		glUniform3fv(hyp->pg.lightclr, hyp->lights.nb, hyp->lights.clr[0]);
	}
	if(hyp->pg.nblights != -1) {
		glUniform1i(hyp->pg.nblights, hyp->lights.nb);
	}
	glUniformMatrix4fv(hyp->pg.mvp, 1, GL_FALSE, hyp->tr.mvp);
	if(hyp->pg.mnormal != -1) {
		glUniformMatrix3fv(hyp->pg.mnormal, 1, 
			GL_FALSE, hyp->tr.mnormal);
	}

//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
	if(iso_line > 0.0)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT, GL_FILL);

	glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, 0, morphvtx);
	glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, 0, clr);

	if(hyp->pg.nrm != -1) {
		glEnableVertexAttribArray(hyp->pg.nrm);
		glVertexAttribPointer(hyp->pg.nrm, 3, GL_FLOAT, 
			GL_FALSE, 0, nrm);
	}

	glDrawElements(GL_TRIANGLES, nbidx*3, GL_UNSIGNED_INT, idx);

	if(hyp->pg.nrm != -1) {
		glDisableVertexAttribArray(hyp->pg.nrm);
	}

goto END;
/* show normals */

float nrmlines[MAX_VERTICES*6];
float clrlines[MAX_VERTICES*8];
float x, y, z;
int i;

for(i = 0; i < nbvtx; i++) {
	x = morphvtx[i*3];
	y = morphvtx[i*3+1];
	z = morphvtx[i*3+2];

	nrmlines[i*6] = x;
	nrmlines[i*6+1] = y;
	nrmlines[i*6+2] = z;

	clrlines[i*8] = clrlines[i*8+1] = clrlines[i*8+2] = clrlines[i*8+3] = 1.0;

	nrmlines[i*6+3] = x + nrm[i*3] * 0.4;
	nrmlines[i*6+4] = y + nrm[i*3+1] * 0.4;
	nrmlines[i*6+5] = z + nrm[i*3+2] * 0.4;

	clrlines[i*8+4] = clrlines[i*8+5] = clrlines[i*8+6] = clrlines[i*8+7] = 1.0;
}
glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, 0, nrmlines);
glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, 0, clrlines);
glDrawArrays(GL_LINES, 0, nbvtx*2); 

/* end show normals */
END:

	glDisableVertexAttribArray(hyp->pg.clr);
	glDisableVertexAttribArray(hyp->pg.pos);

//	glDisable(GL_CULL_FACE);
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "opengl.h"
#include "hypnomix.h"
#include "math.h"


char modname[] = "parasite";

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
static GLfloat tex[MAX_VERTICES * 2];
static GLfloat morphvtx[MAX_VERTICES * 3];
static GLfloat nrm[MAX_VERTICES * 3]; /* one normal per vertice */
static int nbidx;
static GLuint idx[MAX_FACES * 3];
static GLfloat clr[MAX_VERTICES * 4];

static void lineMiddle(struct point3D *a, struct point3D *b, struct point3D *m)
{
	m->x = (b->x + a->x) / 2.0;
	m->y = (b->y + a->y) / 2.0;
	m->z = (b->z + a->z) / 2.0;
}

static int nbmatch = 0;

static GLuint vertexExists(GLfloat *vtx, int nbvtx, struct point3D *v)
{
	int i = 0;
	do {
		if(FLOATCMP(vtx[i*3], v->x) && FLOATCMP(vtx[i*3+1], v->y) 
			&& FLOATCMP(vtx[i*3+2], v->z))
			break;
		i++;
	} while(i < nbvtx);

if(i < nbvtx)
	nbmatch++;

	return i;	
}


static void addVertex(GLfloat *vtx, GLuint *idx, int *nbvtx, int nbidx,
	struct point3D *v) 
{
	float len;
	int i;

	len = vec3Mag((GLfloat *)v);
	if(len > 0) {
		v->x /= len;
		v->y /= len;
		v->z /= len;
	}
	if((i = vertexExists(vtx, *(nbvtx), v)) < (*nbvtx))
		idx[nbidx] = i;
	else {
		idx[nbidx] = (*nbvtx);
		vtx[(*nbvtx)*3] = v->x;
		vtx[(*nbvtx)*3+1] = v->y;
		vtx[(*nbvtx)*3+2] = v->z;
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



void isocahedronNormals(void)
{
	int i;
	GLfloat *a, *b, *c;
	GLuint v1, v2, v3;
	vec3 vnormal;	
	vec3 radius;

	bzero(nrm, nbvtx * 3 * sizeof(GLfloat));

	for(i = 0; i < nbidx; i++) {
		v1 = idx[i*3];
		v2 = idx[i*3+1];
		v3 = idx[i*3+2];

		a = morphvtx + v1 * 3;
		b = morphvtx + v2 * 3;
		c = morphvtx + v3 * 3;

		/* one normal per vertices and three vertices per face */
		vec3Normal(vnormal, a, b, c);
/*
		radius[0] = a[0];
		radius[1] = a[1];
		radius[2] = a[2];
		vec3Normalize(radius); */
/*
	nrm[v1*3] = vnormal[0];
	nrm[v1*3+1] = vnormal[1];
	nrm[v1*3] = vnormal[0];
	nrm[v2*3+1] = vnormal[1];
	nrm[v2*3+2] = vnormal[2];
	nrm[v2*3] = vnormal[0];
	nrm[v3*3+1] = vnormal[1];
	nrm[v3*3+2] = vnormal[2];
	nrm[v3*3+2] = vnormal[2]; */

		nrm[v1*3] = (nrm[v1*3] + vnormal[0]) / 2.0;
		nrm[v1*3+1] = (nrm[v1*3+1] + vnormal[1]) / 2.0;
		nrm[v1*3+2] = (nrm[v1*3+2] + vnormal[2]) / 2.0;

		nrm[v2*3] = (nrm[v2*3] + vnormal[0]) / 2.0;
		nrm[v2*3+1] = (nrm[v2*3+1] + vnormal[1]) / 2.0;
		nrm[v2*3+2] = (nrm[v2*3+2] + vnormal[2]) / 2.0;

		nrm[v3*3] = (nrm[v3*3] + vnormal[0]) / 2.0;
		nrm[v3*3+1] = (nrm[v3*3+1] + vnormal[1]) / 2.0;
		nrm[v3*3+2] = (nrm[v3*3+2] + vnormal[2]) / 2.0; 

/* ADD NORMALIZE !!!! */
tex[v1*3] = 0.0;
tex[v1*3+1] = 0.0;
tex[v2*3] = 1.0;
tex[v2*3+1] = 1.0;
tex[v3*3] = 0.0;
tex[v3*3+1] = 1.0;
	}
}


static void morph(struct hypnomix *hyp)
{
	int i, j;
	GLfloat mul;

	i = 0;
	j = 0;
	while(i < nbvtx) {
		mul = pow(1.0 + hyp->morphsamples[j++], 2.0);

		if(j >= hyp->nbsamples)
			j = 0;

		morphvtx[i*3] = vtx[i*3] * mul;
		morphvtx[i*3+1] = vtx[i*3+1] * mul;
		morphvtx[i*3+2] = vtx[i*3+2] * mul;
		i++;
	}

	isocahedronNormals();
}


float iso_line = 0.0;


void init(struct hypnomix *hyp)
{
	int nbsubs = 2;
	int face;
	int i;
	int sub;
	GLuint v1, v2, v3;
	struct point3D a, b, c;
	struct point3D mab, mac, mbc;
	GLfloat *vtxtmp;
	GLuint *idxtmp;
	int nbvtxtmp, nbidxtmp;

	levels = nbsubs;

	transformProjection(&hyp->tr, hyp->pmatrix);
	hyp->var.nbbands = 3;

fprintf(stderr, "\n\nPARASITE INIT\n\n");

float red, green, blue;

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
	for(i = 0; i < nbidx; i++) {
		red = (float)rand() / (float)RAND_MAX;
		green = (float)rand() / (float)RAND_MAX;
		blue = (float)rand() / (float)RAND_MAX;


red = green = 1.0;
blue = 0.5; 

		clr[idx[i*3]*4] = red;
		clr[idx[i*3]*4+1] = green; 
		clr[idx[i*3]*4+2] = blue;
		clr[idx[i*3]*4+3] = 1.0;

		clr[idx[i*3+1]*4] = red; 
		clr[idx[i*3+1]*4+1] = green; 
		clr[idx[i*3+1]*4+2] = blue;
		clr[idx[i*3+1]*4+3] = 1.0;

		clr[idx[i*3+2]*4] = red; 
		clr[idx[i*3+2]*4+1] = green; 
		clr[idx[i*3+2]*4+2] = blue;
		clr[idx[i*3+2]*4+3] = 1.0;
	}


// sortVertices(vtx, nbvtx, idx, nbidx);

	memcpy(morphvtx, vtx, nbvtx * sizeof(GLfloat) * 3);
	
	isocahedronNormals();
}


void destroy(struct hypnomix *hyp)
{
	/* nothing to do */
fprintf(stderr, "\n\nPARASITE DESTROY\n\n");
}


void draw(struct hypnomix *hyp)
{
	vec3 lightdir;
	vec3 lightpos;
	float ax, ay;

	morph(hyp);

	ax = hyp->var.smooth[0] * hyp->pr.var[0] * M_PI;
	ay = hyp->var.smooth[1] * hyp->pr.var[1] * M_PI;

/*
fprintf(stderr, "[%d - smooth=%f varx=%f ax=%f]\n", 0, hyp->var.smooth[0], hyp->pr.varx, ax);
fprintf(stderr, "[%d - smooth=%f vary=%f ay=%f]\n", 1, hyp->var.smooth[1], hyp->pr.vary, ay); */


	//az = hyp->var.smoothh * M_PI;

	transformIdentity(&hyp->tr);
	//transformModelTranslate(&hyp->tr, 0.0, 0.0, -6.0);
	transformModelRotate(&hyp->tr, ax, ay, 0.0);
	// transformViewRotate(&hyp->tr, ay, 0.0, 0.0);
	transformViewTranslate(&hyp->tr, 0.0, 0.0, -6.0);
	transformMVP(&hyp->tr);

	glUseProgram(hyp->pg.id);
	glEnableVertexAttribArray(hyp->pg.pos);
	glEnableVertexAttribArray(hyp->pg.clr);
	glEnableVertexAttribArray(hyp->pg.nrm);
/*
	transformMVCoordinates(&hyp->tr, &(hyp->lights.dir[0]), lightdir);
	glUniform3fv(hyp->pg.lightdir, hyp->lights.nb, lightdir);
	transformMVCoordinates(&hyp->tr, &(hyp->lights.pos[0]), lightpos);
	glUniform3fv(hyp->pg.lightpos, hyp->lights.nb, lightpos);
*/
	glUniformMatrix4fv(hyp->pg.mvp, 1, GL_FALSE, hyp->tr.mvp);
	if(hyp->pg.mnormal != -1) {
		glUniformMatrix3fv(hyp->pg.mnormal, 1, 
			GL_FALSE, hyp->tr.mnormal);
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
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

//glEnableVertexAttribArray(a_color_texture);
//glVertexAttribPointer(a_color_texture, 2, GL_FLOAT, GL_FALSE, 0, tex);
//glActiveTexture(GL_TEXTURE0);
//glUniform1i(u_color_texture0, 0);
//glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FUN]);
//glClearColor(1.0, 0.0, 0.0, 1.0);
//glClear(GL_COLOR_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, nbidx*3, GL_UNSIGNED_INT, idx);

	if(hyp->pg.nrm != -1) {
		glDisableVertexAttribArray(hyp->pg.nrm);
	}

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


	glDisableVertexAttribArray(hyp->pg.clr);
	glDisableVertexAttribArray(hyp->pg.pos);

// glDisableVertexAttribArray(a_color_texture);

	glDisable(GL_CULL_FACE);
}

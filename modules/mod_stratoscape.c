#include <stdio.h>
#include <string.h>
#include <math.h>

#include "hypnomix.h"

char modname[] = "stratoscape";


#define NB_CIRCLES 10
#define NB_VTX_PER_CIRCLE 32 
#define CIRCLE_RADIUS 0.5 
#define ORGZ 0.0
#define INCZ 1.0


static float vtx[NB_CIRCLES*NB_VTX_PER_CIRCLE*3];
static float clr[NB_CIRCLES*NB_VTX_PER_CIRCLE*4];
static float nrm[NB_CIRCLES*NB_VTX_PER_CIRCLE*3]; /* as many normals as vtx */
static int nbvtx = 0;
static int idvtx = 0;
static unsigned int idx[NB_CIRCLES*NB_VTX_PER_CIRCLE*3*2]; /*FIXME: is it enough ? */
static int nbidx = 0;
static int ididx = 0;


static void buildCircle(struct hypnomix *hyp, float r, float x, float y, float z)
{
	int i, j;
	float a = hyp->var.average;
	float ainc = M_PI * 2 / NB_VTX_PER_CIRCLE;
	float *v = vtx + idvtx * 3;
	float *c = clr + idvtx * 4;
	float tmpr;

//fprintf(stderr, "BUILD LINE\n");

	j = 0;
	for(i = 0; i < NB_VTX_PER_CIRCLE; i++) {
//fprintf(stderr, "idvtx=%d, <%f>", idvtx, hyp->samples[j]);
		tmpr = r - hyp->samples[j] / (2.0 / r);
		v[i*3] = x + tmpr * cos(a);
		v[i*3+1] = y + tmpr * sin(a);
		v[i*3+2] = z;

		c[i*4] = hyp->samples[j] * hyp->var.delta[0];
		c[i*4+1] = hyp->samples[j] * hyp->var.delta[1];
		c[i*4+2] = hyp->samples[j] * hyp->var.delta[2];
		c[i*4+3] = 1.0;

		j += hyp->nbsamples / NB_VTX_PER_CIRCLE;
		if(j > hyp->nbsamples) {
			j = 0;
		}
		a += ainc;
		idvtx++;
	}
}


static void computeNormals(unsigned int id)
{
	unsigned int v1, v2, v3;
	float *a, *b, *c;
	vec3 vnormal;

	v1 = idx[id*3];
	v2 = idx[id*3+1];
	v3 = idx[id*3+2];
//fprintf(stderr, "/%d,%d,%d/ ", v1, v2, v3);

	a = vtx + v1 * 3;
	b = vtx + v2 * 3;
	c = vtx + v3 * 3;

	vec3Normal(vnormal, a, b, c);

	nrm[v1*3] = vnormal[0];
	nrm[v1*3+1] = vnormal[1];
	nrm[v1*3+2] = vnormal[2];

	nrm[v2*3] = vnormal[0];
	nrm[v2*3+1] = vnormal[1];
	nrm[v2*3+2] = vnormal[2];

	nrm[v3*3] = vnormal[0];
	nrm[v3*3+1] = vnormal[1];
	nrm[v3*3+2] = vnormal[2]; 
}


static void buildCylinder(unsigned int first, unsigned int last)
{
	int i;

// //fprintf(stderr, "ididx=%d, idvtx=%d F=%d L=%d\n", ididx, idvtx, first, last);
	for(i = 0; i < NB_VTX_PER_CIRCLE-1; i++) {
		idx[ididx*3] = i + first;
		idx[ididx*3+1] = i + first + 1;
		idx[ididx*3+2] = i + last;
		computeNormals(ididx);
		ididx++;
		idx[ididx*3] = i + last;
		idx[ididx*3+1] = i + first + 1;
		idx[ididx*3+2] = i + last + 1;
		computeNormals(ididx);
		ididx++;
	}
	/* close the cylinder */
	idx[ididx*3] = first + NB_VTX_PER_CIRCLE-1;
	idx[ididx*3+1] = first;
	idx[ididx*3+2] = last + NB_VTX_PER_CIRCLE-1;
	computeNormals(ididx);
	ididx++;
	idx[ididx*3] = last + NB_VTX_PER_CIRCLE-1;
	idx[ididx*3+1] = first;
	idx[ididx*3+2] = last;
	computeNormals(ididx);
	ididx++; 
}


static void addCylinder(struct hypnomix *hyp, float r, float x, float y, float z)
{
	unsigned int first, last;

	if(ididx >= nbidx) {
		ididx = 0;
	}

	if(idvtx >= nbvtx) {
		idvtx = 0;
//		ididx = 0;
		first = (NB_CIRCLES-1) * NB_VTX_PER_CIRCLE;
		last = 0;
//fprintf(stderr, "OLALAH\n");
	} else {
		first = idvtx - NB_VTX_PER_CIRCLE;
		last = idvtx;
	}
	buildCircle(hyp, r, x, y, z);
	buildCylinder(first, last);
}


void init(struct hypnomix *hyp)
{
	int i;

	transformProjection(&hyp->tr, hyp->pmatrix);
	hyp->var.nbbands = 3;

	nbvtx = NB_CIRCLES*NB_VTX_PER_CIRCLE;
	/* bzero(vtx, sizeof(float)*nbvtx*3);
	bzero(clr, sizeof(float)*nbvtx*4); */
	idvtx = 0;
	buildCircle(hyp, CIRCLE_RADIUS, 0.0, 0.0, ORGZ);
	
	ididx = 0;
	for(i = 0; i < NB_CIRCLES-1; i++) {
		buildCircle(hyp, CIRCLE_RADIUS, 0.0, 0.0, ORGZ-i*INCZ);
		buildCylinder(i*NB_VTX_PER_CIRCLE, (i+1)*NB_VTX_PER_CIRCLE);
//		addCylinder(hyp, CIRCLE_RADIUS, 0.0, 0.0, -2.0-i);
	}	
	nbidx = ididx;
//	ididx = 0; 
for(i = 0; i < nbidx; i++) {
//fprintf(stderr, "%d[%d,%d,%d]\n", i, idx[i*3], idx[i*3+1], idx[i*3+2]);
}
/*
static float orgz = -1.0 - NB_CIRCLES;
addCylinder(hyp, CIRCLE_RADIUS, 0.0, 0.0, --orgz);
// addCylinder(hyp, CIRCLE_RADIUS, 0.0, 0.0, --orgz);
for(i = 0; i < nbidx; i++) {
//fprintf(stderr, "%d[%d,%d,%d]\n", i, idx[i*3], idx[i*3+1], idx[i*3+2]);
}
//addCylinder(hyp, CIRCLE_RADIUS, 0.0, 0.0, --orgz);
for(i = 0; i < nbidx; i++) {
//fprintf(stderr, "%d[%d,%d,%d]\n", i, idx[i*3], idx[i*3+1], idx[i*3+2]);
}
//addCylinder(hyp, CIRCLE_RADIUS, 0.0, 0.0, --orgz);
//fprintf(stderr, "NBVTX=%d, NBIDX=%d\n", nbvtx, nbidx);

for(i = 0; i < nbidx; i++) {
//fprintf(stderr, "%d[%d,%d,%d]\n", i, idx[i*3], idx[i*3+1], idx[i*3+2]);
} */

for(i = 0; i < nbvtx; i++) {
	//fprintf(stderr, "#%f,%f,%f", nrm[i*3], nrm[i*3+1], nrm[i*3+2]);
}
}


void destroy(struct hypnomix *hyp)
{
	/* nothing to do */
}


static float translatez = 0.0;
static float orgz = ORGZ - NB_CIRCLES;
static float addfreq = 1.0;


void draw(struct hypnomix *hyp)
{
//translatez = 4.0;
	transformIdentity(&hyp->tr);
	transformModelTranslate(&hyp->tr, 0.0, 0.0, translatez);
	transformMVP(&hyp->tr);
	translatez += 0.05;
	addfreq -= 0.05;

// //fprintf(stderr, "addfreq=%f translatez=%f\n", addfreq, translatez);

	if(addfreq < 0.0) {	
		addCylinder(hyp, CIRCLE_RADIUS, 0.0, 0.0, orgz);
		orgz -= INCZ;
// //fprintf(stderr, "addfreq < 0.0\n");
		addfreq = 1.0;
	}

// glEnable(GL_CULL_FACE);
// glCullFace(GL_BACK);



// lightdir[0] = tmp[0]; lightdir[1] = tmp[1]; lightdir[2] = tmp[2];

/*fprintf(stderr, " / ");
for(i = 0; i < 4; i++) fprintf(stderr, "%f ", lightdir[i]);
fprintf(stderr, "\n"); */
// mat4Dump(modgeography.tr.view);

	glUseProgram(hyp->pg.id);

	glUniformMatrix4fv(hyp->pg.mvp, 1, GL_FALSE, hyp->tr.mvp);
	glUniformMatrix3fv(hyp->pg.mnormal, 1, GL_FALSE, hyp->tr.mnormal);
//	glUniformMatrix4fv(hyp->pg.mmodel, 1, GL_FALSE, mmodel);

	glEnableVertexAttribArray(hyp->pg.pos);
	glEnableVertexAttribArray(hyp->pg.clr);
	glEnableVertexAttribArray(hyp->pg.nrm);
/*
	vec3 lightdir;
	vec3 lightpos;
	transformMVCoordinates(&hyp->tr, &(hyp->lights.dir[0]), lightdir);
	transformMVCoordinates(&hyp->tr, &(hyp->lights.pos[0]), lightpos);
	glUniform3fv(hyp->pg.lightdir, hyp->lights.nb, lightdir);
	glUniform3fv(hyp->pg.lightdir, hyp->lights.nb, lightdir); */

	glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, 0, vtx);
	glVertexAttribPointer(hyp->pg.nrm, 3, GL_FLOAT, GL_FALSE, 0, nrm);
	glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, 0, clr);

// glDrawArrays(GL_POINTS, 0, nbvtx);
//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, nbidx*3, GL_UNSIGNED_INT, idx);
	// glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, idx+(NB_CIRCLES-1)*3);


//

	glDisableVertexAttribArray(hyp->pg.nrm);
	glDisableVertexAttribArray(hyp->pg.clr);
	glDisableVertexAttribArray(hyp->pg.pos);

// glDisable(GL_CULL_FACE);
}

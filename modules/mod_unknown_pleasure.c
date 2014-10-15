#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opengl.h"
#include "hypnomix.h"


char modname[] = "unknown pleasure";


#define MAX_GRID_WIDTH 64
#define MAX_GRID_HEIGHT 80
#define NB_SAMPLES_BUFFER 4
#define GRID_HEIGHT_SCALE 0.6 
#define GRID_WIDTH_SCALE GRID_HEIGHT_SCALE * 0.81
#define MAX_POW 6.5

static float vtx[MAX_GRID_WIDTH*MAX_GRID_HEIGHT*3*2];
static GLuint idx[MAX_GRID_WIDTH*MAX_GRID_HEIGHT*3*2];
static float morphvtx[MAX_GRID_WIDTH*MAX_GRID_HEIGHT*3*2];
static GLfloat clr[MAX_GRID_WIDTH*MAX_GRID_HEIGHT*4];
static GLfloat clrbck[MAX_GRID_WIDTH*MAX_GRID_HEIGHT*4*2];
static float *samplesbuf;

static int gridw = 32.0;
static int gridh = 64.0;


void init(struct hypnomix *hyp)
{
	int i, j, id;
	float orgx, orgy, orgz;
	float x, y, z;
	float w, h;
	mat4 mortho;
	int offsetvtx, offsetclr;
	int nbidx;

	mat4Orthographic(mortho, 0.0, hyp->w, 0.0, hyp->h, -1.0, hyp->h);
	transformProjection(&hyp->tr, mortho);
	hyp->var.nbbands = 3;

	samplesbuf = malloc(hyp->nbsamples*sizeof(float)*NB_SAMPLES_BUFFER);
	bzero(samplesbuf, hyp->nbsamples*sizeof(float)*NB_SAMPLES_BUFFER);

	if(gridw > MAX_GRID_WIDTH) {
		gridw = MAX_GRID_WIDTH;
	}
	if(gridh > MAX_GRID_HEIGHT) {
		gridh = MAX_GRID_HEIGHT;
	}
	w = (hyp->w / gridw) * GRID_WIDTH_SCALE * ((float)hyp->h / (float)hyp->w);
	h = (hyp->h / gridh) * GRID_HEIGHT_SCALE;

	orgx = (hyp->w / 2.0) - (w / 2.0) * (gridw-1);
	orgy = (hyp->h / 2.0) - (h / 2.0) * (gridh-1);

fprintf(stderr, "HY = %d, H = %f, GH = %d, OY = %f\n", hyp->h, h, gridh, orgy);
	orgz = -gridh;
	offsetvtx = gridw * gridh * 3;
	offsetclr = gridw * gridh * 4;

	for(j = 0; j < gridh; j++) {
		for(i = 0; i < gridw; i++) {
			x = orgx + (float)i * w;
			y = orgy + (float)j * h;
			z = orgz + (float)j;

			/* lines */
			id = (i + j * gridw) * 3;
			setVtx(&vtx[id], x, y, z);
			id = (i + j * gridw) * 4;
			setClr(&clr[id], hyp->pr.clr0[0], hyp->pr.clr0[1], 
				hyp->pr.clr0[2], hyp->pr.clr0[3]);
			setClr(&clrbck[id], 0.0, 0.0, 0.0, 1.0);

			/* fill */
			id = (i + j * gridw) * 3 + offsetvtx;
			setVtx(&vtx[id], x, y, z);
			id = (i + j * gridw) * 4 + offsetclr;
			setClr(&clrbck[id], 0.0, 0.0, 0.0, 1.0);
		}
	}

	nbidx = 0;
	for(j = 0; j < gridh; j++) {
		for(i = 0; i < gridw-1; i++) {
			id = j * gridw;
			idx[nbidx++] = i + id;
			idx[nbidx++] = i + id + 1;
			idx[nbidx++] = i + id + gridw * gridh;
			idx[nbidx++] = i + id + gridw * gridh;
			idx[nbidx++] = i + id + 1;
			idx[nbidx++] = i + id + gridw * gridh + 1;
		}
	}

	memcpy(morphvtx, vtx, gridw*gridh*6*sizeof(float));
}


void destroy(struct hypnomix *hyp)
{
	free(samplesbuf);
}


static void morph(struct hypnomix *hyp)
{
	static int nbuf = 0;
	int i, j, k, l;
	int id;
	float tmp, p, pj;
	float middle = gridw / 2.0;

	for(i = 0; i < hyp->nbsamples; i++) {
		samplesbuf[i*4+nbuf] = hyp->morphsamples[i];
	}
	if(++nbuf >= NB_SAMPLES_BUFFER) {
		nbuf = 0;
	};

	k = 0;
	l = 0;
	for(j = 0; j < gridh; j++) {
		if(j < (gridh / 2)) {
			pj = (float)j * MAX_POW / gridh;
		} else {
			pj = (gridh-j) * MAX_POW / gridh;
		} 
		for(i = 0; i < gridw; i++) {
			id = (i + j * gridw) * 3 + 1;

			if(i < middle) {
				p = pj + (float)i * MAX_POW / middle;
			} else {
				p = pj + (gridw-i) * MAX_POW / middle;
			}
			if(i > gridw / 4 && i < 3 * gridw / 4) {
				tmp = exp(samplesbuf[k]);
				if(++k > hyp->nbsamples*NB_SAMPLES_BUFFER) {
					k = 0;
				}
			} else {
				tmp = exp(samplesbuf[l]);
				if(++l > hyp->nbsamples*NB_SAMPLES_BUFFER) {
					l = 0;
				}
			}
			tmp = pow(tmp, p) - 1.0;
			morphvtx[id] = vtx[id] - tmp; 
		}
	}
}


void draw(struct hypnomix *hyp)
{
	int i, j, id;

	morph(hyp);

	transformIdentity(&hyp->tr);
	// transformViewTranslate(&hyp->tr, 0.0, 0.0, -4.0);
	transformMVP(&hyp->tr);

	glUseProgram(hyp->pg.id);
	glEnableVertexAttribArray(hyp->pg.pos);
	glEnableVertexAttribArray(hyp->pg.clr);
//		glUniform3fv(mod.pg.lightdir, hyp->lights.nb, hyp->lights.dir);
	//glUniform3fv(hyp->pg.lightclr, hyp->lights.nb, hyp->lights.clr);
	//glUniform1i(hyp->pg.nblights, hyp->lights.nb);

	glUniformMatrix4fv(hyp->pg.mvp, 1, GL_FALSE, hyp->tr.mvp);
	//glUniformMatrix3fv(hyp->pg.mnormal, 1, GL_FALSE, hyp->tr.mnormal);

	glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, 0, morphvtx);
// glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, 0, vtx);
	glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, 0, clrbck);

	for(j = 0; j < gridh; j++) {
		glDrawElements(GL_TRIANGLE_STRIP, (gridw-1)*6, GL_UNSIGNED_INT,
			(char *)idx+j*(gridw-1)*6*sizeof(GLuint));
	}

	glLineWidth(1.0);
	for(j = 0; j < gridh; j++) {
		for(i = 0; i < gridw; i++) {
			id = (i + j * gridw) * 3 + 1;
			morphvtx[id] -= 1.0; 
		}
	}
	glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, 0, clr);
	for(j = 0; j < gridh; j++) {
		glDrawArrays(GL_LINE_STRIP, gridw*j, gridw);
//fprintf(stderr, "%f ", vtx[gridw*j*3+2]);
	}
//fprintf(stderr, "\n\n");
//	glDrawArrays(GL_POINTS, 0, 480*3);

	glDisableVertexAttribArray(hyp->pg.clr);
	glDisableVertexAttribArray(hyp->pg.pos);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hypnomix.h"
#include "obj.h"
#include "presets.h"

char modname[] = "kinetics";

static char *objs[] = { "sphere.obj", "cube.obj", "minifig.obj", "leia.obj", "ORCA.OBJ", "WonderWoman.obj", NULL };


static struct obj obj;

static float *clr;


void init(struct hypnomix *hyp)
{
	int nbobj = 0;
	int id;
	float tmp;
	char *path;
	int i;	
	char *objname;

	transformProjection(&hyp->tr, hyp->pmatrix);
	hyp->var.nbbands = 3;

	do {
	} while(objs[++nbobj] != NULL);
	
//	id = rand() % nbobj; 
//id = 2; /* FIXME: use only triangles .obj */
	presetGetStr(&hyp->pr, "objname", &objname);
/*
	path = malloc(strlen(hyp->home) + strlen(objs[id]) + 1);
	strcpy(path, hyp->home);
	strcat(path, objs[id]); */
	
	path = malloc(strlen(hyp->home) + strlen(objname) + 1);
	strcpy(path, hyp->home);
	strcat(path, objname); 
	objInit(&obj, path);
	(*hyp->log)(HYP_INFO, "load meshes: %s\n", objname);
	free(path);

	transformComputeAverageNormals(obj.idx, obj.nbidx, 
		obj.vtx, obj.nbvtx, obj.nrm, 3); 

	clr = malloc(sizeof(float)*4*obj.nbvtx);
	for(i = 0; i < obj.nbvtx; i++) {
		clr[i*4] = 1.0;
		clr[i*4+1] = 1.0;
		clr[i*4+2] = 1.0;
		clr[i*4+3] = 1.0;
	}
}


void destroy(struct hypnomix *hyp)
{
	objDestroy(&obj);
	free(clr);
}


/* FIXME: enhancement: draw several objects at the same time with some little variation */
void draw(struct hypnomix *hyp)
{
	float ax, ay, az;
	int id;
	vec3 light; 

	ax = hyp->var.smooth[0] * M_PI * hyp->pr.var[0];
	ay = hyp->var.smooth[1] * M_PI * hyp->pr.var[1];
	az = hyp->var.smooth[2] * M_PI * hyp->pr.var[2];

	transformIdentity(&hyp->tr);
	transformModelTranslate(&hyp->tr, 0.0, -obj.centery, -obj.scalingz);
static float angle = 0.0;
	transformModelRotate(&hyp->tr, 0.0, angle, 0.0);
angle += 0.01;

	transformViewTranslate(&hyp->tr, 0.0, 0.0, -2.0);
	// transformViewRotate(&hyp->tr, 0.0, angle, 0.0);

	transformMVP(&hyp->tr);

//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	glUseProgram(hyp->pg.id);
	glUniformMatrix4fv(hyp->pg.mvp, 1, GL_FALSE, hyp->tr.mvp);
	glUniformMatrix3fv(hyp->pg.mnormal, 1, GL_FALSE, hyp->tr.mnormal);
//	glUniformMatrix4fv(hyp->pg.mmodel, 1, GL_FALSE, mmodel);

	glEnableVertexAttribArray(hyp->pg.pos);
	glEnableVertexAttribArray(hyp->pg.clr);
	glEnableVertexAttribArray(hyp->pg.nrm);

	transformToView(&hyp->tr, hyp->lights.dir[0], light);
	glUniform3fv(hyp->pg.lightdir, hyp->lights.nb, light);

	glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, 0, obj.vtx);
	glVertexAttribPointer(hyp->pg.nrm, 3, GL_FLOAT, GL_FALSE, 0, obj.nrm);
	glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, 0, obj.clr);

	// glDrawArrays(GL_TRIANGLES, 0, obj.nbvtx);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
 	glDrawElements(GL_TRIANGLES, obj.nbidx*3, GL_UNSIGNED_INT, obj.idx);

	id = obj.nbidx * hyp->var.band[0];
	if(id > obj.nbidx) {
		id = obj.nbidx;
	}
	
	glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, 0, clr);
// 	glDrawElements(GL_LINES, id*3, GL_UNSIGNED_INT, obj.idx);

//	glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, 0, obj.vtx);
//	glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, 0, obj.clr);
/*
unsigned int nb = (unsigned int)(hyp->var.smootha*obj.nbvtx);
	glDrawArrays(GL_LINES, 0, nb);
fprintf(stderr, "%d ", nb); */
glDisable(GL_CULL_FACE);


/* show normals */

goto END;

float nrmlines[10000*6];
float clrlines[10000*8];
float x, y, z;
int i;

for(i = 0; i < obj.nbvtx; i++) {
	x = obj.vtx[i*3];
	y = obj.vtx[i*3+1];
	z = obj.vtx[i*3+2];

	nrmlines[i*6] = x;
	nrmlines[i*6+1] = y;
	nrmlines[i*6+2] = z;

	clrlines[i*8] = clrlines[i*8+1] = clrlines[i*8+2] = clrlines[i*8+3] = 1.0;

	nrmlines[i*6+3] = x + obj.nrm[i*3] * 0.4;
	nrmlines[i*6+4] = y + obj.nrm[i*3+1] * 0.4;
	nrmlines[i*6+5] = z + obj.nrm[i*3+2] * 0.4;

	clrlines[i*8+4] = clrlines[i*8+5] = clrlines[i*8+6] = clrlines[i*8+7] = 1.0;
}
glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, 0, nrmlines);
glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, 0, clrlines);
glDrawArrays(GL_LINES, 0, obj.nbvtx*2); 
/* end show normals */

END:

	glDisableVertexAttribArray(hyp->pg.nrm);
	glDisableVertexAttribArray(hyp->pg.clr);
	glDisableVertexAttribArray(hyp->pg.pos);
//glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

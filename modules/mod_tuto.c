#include <stdio.h>
#include <string.h> /* for bzero: to move to transform */

#include "hypnomix.h"
#include "skybox.h"


#define MAGICNB 0.7071067814 /* 1 / sqrt(2), https://en.wikipedia.org/wiki/Tetrahedron */

static float vtx[] = {
        1.0, 0.0, -MAGICNB,
        -1.0, 0.0, -MAGICNB,
        0.0, 1.0, MAGICNB,
        0.0, -1.0, MAGICNB
};

static unsigned int idx[] = {
        0, 1, 2,
        1, 3, 2,
        0, 2, 3,
        0, 3, 1
};

static float nrm[12];

static float clr[] = {
/*        1.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
	1.0, 0.0, 1.0, 1.0 */
	0.0, 0.0, 1.0, 1.0,
	0.0, 0.0, 1.0, 1.0,
	0.0, 0.0, 1.0, 1.0,
	0.0, 0.0, 1.0, 1.0
};



char modname[] = "tuto";

/*
void transformComputeAverageNormalsFromIdx(unsigned int *idx, int nbidx,
	float *vtx, int nbvtx)
{	
	unsigned int v1, v2, v3;
	float *a, *b, *c;
	vec3 vnormal;
	int i;

	bzero(nrm, nbvtx*3*sizeof(float));
	for(i = 0; i < nbidx; i++) {
		v1 = idx[i*3];
		v2 = idx[i*3+1];
		v3 = idx[i*3+2];

		a = vtx + v1 * 3;
		b = vtx + v2 * 3;
		c = vtx + v3 * 3;	
		vec3Normal(vnormal, a, b, c);

		nrm[v1*3] += vnormal[0];
		nrm[v1*3+1] += vnormal[1];
		nrm[v1*3+2] += vnormal[2];

		nrm[v2*3] += vnormal[0];
		nrm[v2*3+1] += vnormal[1];
		nrm[v2*3+2] += vnormal[2];

		nrm[v3*3] += vnormal[0];
		nrm[v3*3+1] += vnormal[1];
		nrm[v3*3+2] += vnormal[2];
	}
	for(i = 0; i < nbvtx; i++) {
		nrm[i*3] /= 3.0;
		vec3Normalize(&nrm[i*3]);
		nrm[i*3+1] /= 3.0;
		vec3Normalize(&nrm[i*3+1]);
		nrm[i*3+2] /= 3.0;
		vec3Normalize(&nrm[i*3+2]);
	}
} */


void init(struct hypnomix *hyp)
{
	transformProjection(&hyp->tr, hyp->pmatrix);
	hyp->var.nbbands = 3;
	skyboxInit(hyp);
	fprintf(stderr, "INFO: module init\n");
	transformComputeAverageNormals(idx, 4, vtx, 4, nrm, 3);
}


void destroy(struct hypnomix *hyp)
{
	skyboxDestroy(hyp);
	fprintf(stderr, "INFO: module destroy\n");
}


void draw(struct hypnomix *hyp)
{
	float angle = hyp->var.smooth[0] * 2.0 * M_PI; // * hyp->pr.var[0];
	float translate = hyp->var.smooth[1] * hyp->pr.var[1];
	float scale = hyp->var.smooth[2] * hyp->pr.var[2];

	float facevtx[] = {
		-1.0, -1.0, 0.0,
		1.0, -1.0, 0.0,
		-1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
	};
	float facenrm[] = {
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0
	};
	float faceclr[] = {
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0 
	}; 
	unsigned int faceidx[] = {
		0, 1, 2,
		2, 1, 3
	};
	skyboxDraw(hyp, 0.0, angle, 0.0);

	transformIdentity(&hyp->tr);
//	transformModelScale(&hyp->tr, scale, scale, scale);
	transformModelRotate(&hyp->tr, 0.0, angle, 0.0); 
//	transformViewRotate(&hyp->tr, 0.0, angle, 0.0);
	transformViewTranslate(&hyp->tr, 0.0, 0.0, -4.0);
	transformMVP(&hyp->tr);



	glUseProgram(hyp->pg.id);
	glEnableVertexAttribArray(hyp->pg.pos);
	glEnableVertexAttribArray(hyp->pg.clr);
	glEnableVertexAttribArray(hyp->pg.nrm);

	glUniformMatrix4fv(hyp->pg.mvp, 1, GL_FALSE, hyp->tr.mvp);
	glUniformMatrix3fv(hyp->pg.mnormal, 1, GL_FALSE, hyp->tr.mnormal);

//	transformMVCoordinates(&hyp->tr, &(hyp->lights.dir[0]), lightdir[0]);
//lightdir[0][0] = 0.0;
//lightdir[0][1] = 0.0;
//lightdir[0][2] = 1.0;
//	glUniform3fv(hyp->pg.lightdir, hyp->lights.nb, lightdir[0]);
//	transformMVCoordinates(&hyp->tr, &(hyp->lights.pos[0]), lightpos[0]);
//	glUniform3fv(hyp->pg.lightpos, hyp->lights.nb, lightpos[0]);

vec4 lightdir; 
vec4 tmp;
tmp[0] = hyp->lights.dir[0][0];
tmp[1] = hyp->lights.dir[0][1];
tmp[2] = hyp->lights.dir[0][2];
tmp[3] = 0.0;
	mat4Vec4Mul(hyp->tr.view, tmp, lightdir);
	vec4Normalize(lightdir, lightdir);

/*fprintf(stderr, "LIGHTDIR=%f,%f,%f,%f\n", lightdir[0], lightdir[1], lightdir[2], lightdir[3]); */
glUniform3fv(hyp->pg.lightdir, hyp->lights.nb, lightdir);
//glUniform3fv(hyp->pg.lightpos, hyp->lights.nb, hyp->lights.pos);
/*
	glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, 0, vtx);
	glVertexAttribPointer(hyp->pg.nrm, 3, GL_FLOAT, GL_FALSE, 0, nrm);
	glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, 0, clr);
*/
//glEnable(GL_CULL_FACE);
//glCullFace(GL_BACK);

glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, 0, facevtx);
glVertexAttribPointer(hyp->pg.nrm, 3, GL_FLOAT, GL_FALSE, 0, facenrm);
glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, 0, faceclr);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, faceidx);


//	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, idx);
//glDisable(GL_CULL_FACE);


/* show normals */

float nrmlines[4*6];
float x, y, z;
int i;

for(i = 0; i < 4; i++) {
	x = facevtx[i*3];
	y = facevtx[i*3+1];
	z = facevtx[i*3+2];

	nrmlines[i*6] = x;
	nrmlines[i*6+1] = y;
	nrmlines[i*6+2] = z;

	nrmlines[i*6+3] = x + facenrm[i*3] * 0.4;
	nrmlines[i*6+4] = y + facenrm[i*3+1] * 0.4;
	nrmlines[i*6+5] = z + facenrm[i*3+2] * 0.4;
}
glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, 0, nrmlines);
glDrawArrays(GL_LINES, 0, 8); 
/* end show normals */

	glDisableVertexAttribArray(hyp->pg.nrm);
	glDisableVertexAttribArray(hyp->pg.clr);
	glDisableVertexAttribArray(hyp->pg.pos);
}

#include <math.h>
#include <string.h>

#include "mathematics.h"
#include "transform.h"


#include <stdio.h>
#include <stdlib.h> /* DEBUG */


void transformIdentity(struct transform *t)
{
	mat4Identity(t->view);
	mat4Identity(t->model);
	mat4Identity(t->mvp);
}


void transformProjection(struct transform *t, mat4 project)
{
	mat4Copy(project, t->project);
}


void transformViewRotate(struct transform *t, float ax, float ay, float az)
{
	mat4 tmp;
	
	mat4Copy(t->view, tmp);
	mat4AnglesRotate(t->viewr, ax, ay, az);
	mat4Mul(t->viewr, tmp, t->view);	
}


void transformViewTranslate(struct transform *t, float tx, float ty, float tz)
{
	mat4 tmp;

	mat4Copy(t->view, tmp);
	mat4Translate(t->viewt, tx, ty, tz);
	mat4Mul(t->viewt, tmp, t->view);
}


void transformModelRotate(struct transform *t, float ax, float ay, float az)
{
	mat4 tmp;
	
	mat4Copy(t->model, tmp);
	mat4AnglesRotate(t->modelr, ax, ay, az);
	mat4Mul(t->modelr, tmp, t->model);	
}


void transformModelTranslate(struct transform *t, float tx, float ty, float tz)
{
	mat4 tmp;

	mat4Copy(t->model, tmp);
	mat4Translate(t->modelt, tx, ty, tz);
	mat4Mul(t->modelt, tmp, t->model);
}


void transformModelScale(struct transform *t, float sx, float sy, float sz)
{
	mat4 tmp;

	mat4Copy(t->model, tmp);
	mat4Scale(t->modelt, sx, sy, sz);
	mat4Mul(t->modelt, tmp, t->model);
}


void transformMVP(struct transform *t)
{
	mat3 m3tmp;

	mat4Mul(t->model, t->view, t->mv);
	mat4Mul(t->mv, t->project, t->mvp);

	mat4To3(t->mv, m3tmp);
	mat3InverseTranspose(m3tmp, t->mnormal); 
}


void transformToView(struct transform *tr, vec3 vin, vec3 vout)
{
	vec4 in, out;

	in[0] = vin[0];
	in[1] = vin[1];
	in[2] = vin[2];
	in[3] = 0.0;
	mat4Vec4Mul(tr->view, in, out);
	vec4Normalize(out, out);
	vout[0] = out[0];
	vout[1] = out[1];
	vout[2] = out[2]; 
	// vec3Normalize(vout);
}


void transformComputeAverageNormals(unsigned int *idx, int nbidx,
	float *vtx, int nbvtx, float *nrm, int nrmpervtx)
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
		vec3Normalize(&nrm[i*3], &nrm[i*3]);
	} 
}


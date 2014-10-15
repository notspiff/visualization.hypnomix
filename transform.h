#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__


#include "mathematics.h"


struct transform {
	mat4 viewt;
	mat4 viewr;
	mat4 view;
	mat4 modelt;
	mat4 modelr;
	mat4 models;
	mat4 model;
	mat4 project;
	mat4 mv;
	mat4 mvp;
	mat3 mnormal;
} transform_t;


void transformIdentity(struct transform *t);
void transformProjection(struct transform *t, mat4 project);
void transformViewRotate(struct transform *t, float ax, float ay, float az);
void transformViewTranslate(struct transform *t, float tx, float ty, float tz);
void transformModelRotate(struct transform *t, float ax, float ay, float az);
void transformModelTranslate(struct transform *t, float tx, float ty, float tz);
void transformModelScale(struct transform *t, float sx, float sy, float sz);
void transformMVP(struct transform *t);
void transformToView(struct transform *tr, vec3 vin, vec3 vout);
void transformComputeAverageNormals(unsigned int *idx, int nbidx,
	float *vtx, int nbvtx, float *nrm, int nrmpervtx);

#endif


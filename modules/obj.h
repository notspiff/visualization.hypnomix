#ifndef __OBJ_H__
#define __OBJ_H__


struct obj {
	char *buf;
	int nbvtx;
	float *vtx;
	int nbnrm; /* in the .obj file, vertices and normals count could be different */
	float *nrm;
	float *texpos;
	int nbidx;
	unsigned int *idx;
	float *clr;
	float centery;
	float scalingz;
} obj_t;


void objInit(struct obj *obj, const char *filename);
void objDestroy(struct obj *obj);
void objCount(struct obj *obj);
void objParse(struct obj *obj);


#endif

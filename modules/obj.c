#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>

#include "obj.h"


/* FIXME: very crappy .OBJ parser */



void objRead(struct obj *obj, const char *filename)
{
	FILE *fp;
	struct stat statinfo;

	fp = fopen(filename, "r");
	if(fp == NULL) {
		fprintf(stderr, "objRead(): %s not found...\n", filename);
		exit(EXIT_FAILURE);
	}
	stat(filename, &statinfo);
	fprintf(stderr, "INFO: objRead(): open %s\n", filename);
	obj->buf = (char *)malloc(statinfo.st_size);
	fread(obj->buf, 1, statinfo.st_size, fp);
	fclose(fp);

}


void objCount(struct obj *obj) 
{
	int i, j;
	char c;
	int len = strlen(obj->buf);
	char line[256]; /* FIXME: line do not exceed 256 chars!! */

	obj->nbvtx = 0;
	obj->nbnrm = 0;
	obj->nbidx = 0;

	i = 0;	
	do {
		j = 0;
		while((i < len) && ((c = obj->buf[i]) != '\n')) {
			line[j++] = c;
			i++;
		}
		line[j] = 0;
//fprintf(stderr, "LINE=%s\n", line);
		switch(line[0]) {
			case 'v':
				if(line[1] == 't') {
		//			vtnb++;
				} else if(line[1] == 'n') {
					obj->nbnrm++;
				} else if(line[1] == ' ') {
					obj->nbvtx++;
				}
				break;
			case 'f':
				if(line[1] == ' ') {
					obj->nbidx++;
				}
				break;
		}

		// fprintf(stderr, "line = %s\n", line);
	} while(++i < len);
}


void objParse(struct obj *obj)
{
	int i, j, v, f;
	int len = strlen(obj->buf);
	char line[256];
	char c;
	float miny, maxy;
	float minz, maxz;
	float height;
	float x, y, z;
	unsigned int v1, v2, v3;
	unsigned int t1, t2, t3;
	unsigned int n1, n2, n3;
	int faceread;
	
	miny = maxy = 0.0;
	minz = maxz = 0.0;
	obj->vtx = malloc(sizeof(float)*obj->nbvtx*3);
	obj->nrm = malloc(sizeof(float)*obj->nbvtx*3);
	obj->texpos = malloc(sizeof(float)*obj->nbvtx*3);
	obj->clr = malloc(sizeof(float)*obj->nbvtx*4);
	obj->idx = malloc(sizeof(unsigned int)*obj->nbidx*3); /* FIXME: work only with triangles */

	i = 0;
	v = 0;
	f = 0;

	do {
		j = 0;
		while((i < len) && ((c = obj->buf[i]) != '\n')) {
			line[j++] = c;
			i++;
		}
		line[j] = 0; 

		if(sscanf(line, "v %f %f %f", &x, &y, &z) == 3) {
			obj->vtx[v*3] = x; 
			obj->vtx[v*3+1] = y;
			obj->vtx[v*3+2] = z;

			obj->clr[v*4] = 0.0; /* FIXME: arbitrary colors... */
			obj->clr[v*4+1] = 0.0;
			obj->clr[v*4+2] = 0.5+(float)i / (float)(len/2.0);
			obj->clr[v*4+3] = 1.0;

			if(y < miny) {
				miny = y;
			}
			if(y > maxy) {
				maxy = y;
			}
			if(z < minz) {
				minz = z;
			}
			if(z > maxz) {
				maxz = z;
			}
			
			v++;
		} 
		if(line[0] == 'f' && line[1] == ' ') {
			faceread = 0;	
			v1 = v2 = v3 = 0;
			n1 = n2 = n3 = 0;
			t1 = t2 = t3 = 0;
			if(sscanf(line, "f %d %d %d", &v1, &v2, &v3) == 3) {
				faceread = 1;
			} else if(sscanf(line, "f %d//%d %d//%d %d//%d",
				&v1, &n1, &v2, &n2, &v3, &n3) == 6) {
				faceread = 1;
			} else if(sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
				&v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3)
				== 9) {
				faceread = 1;
			} else if(sscanf(line, "f %d/%d %d/%d %d/%d",
				&v1, &t1, &v2, &t2, &v3, &t3) == 6) {
				faceread = 1;
			}
			if(faceread) {
				/* .obj count vertices from 1 to nbvtx */
				obj->idx[f*3] = v1-1;
				obj->idx[f*3+1] = v2-1;
				obj->idx[f*3+2] = v3-1;
			
// printf("LINE=%s => %d, %d, %d\n", line, v1, v2, v3);
				f++;
			}
		}
	} while(++i < len); // && k < obj->nbvtx);
	
	height = fabs(maxy-miny);

	obj->centery = height / 2.0;
	obj->scalingz = height * 1.5; /* FIXME: arbitrary */ 

fprintf(stderr, "read %d vtx, %d faces, centery=%f, scalingz=%f\n", v, f, obj->centery, obj->scalingz);
}


void objInit(struct obj *obj, const char *filename)
{

fprintf(stderr, "< objRead");
	objRead(obj, filename);
fprintf(stderr, "> objRead");
	objCount(obj);
fprintf(stderr, "nbvtx=%d, nbnrm=%d, nbidx=%d\n", obj->nbvtx, obj->nbnrm, obj->nbidx);
	objParse(obj);
	free(obj->buf);
/*
int i;
int id;
float *vtx;
for(i = 0; i < obj->nbidx; i++) {
	id = obj->idx[i*3];
	vtx = obj->vtx;
	printf("%d) %f, %f, %f\n", id, vtx[id*3], vtx[id*3+1], vtx[id*3+2]);
	id = obj->idx[i*3+1];
	printf("%d) %f, %f, %f\n", id, vtx[id*3], vtx[id*3+1], vtx[id*3+2]);
	id = obj->idx[i*3+2];
	printf("%d) %f, %f, %f\n", id, vtx[id*3], vtx[id*3+1], vtx[id*3+2]);
} */
}


void objDestroy(struct obj *obj)
{
	free(obj->vtx);
	free(obj->nrm);
	free(obj->texpos);
	free(obj->clr);
	free(obj->idx);
}

#ifndef __HYPNOMIX_H__
#define __HYPNOMIX_H__

#include <stdarg.h>

#include "mathematics.h"
#include "shader.h"
#include "transform.h"
#include "presets.h"
#include "dir.h"

#define HYP_ERROR -1
#define HYP_WARNING 0
#define HYP_INFO 1
#define HYP_DEBUG 2
#define HYP_SUCCESS 3

#define MAX_ACTIVE_MODULES 8 

#define MOVEMENT_FREQ 10 /* freq between two variations computation */
#define MOVEMENT_SPEED 0.2 
#define MAX_LIGHT 2

#define MAX_BAND 16


struct variation {
	int refresh, refreshfq;
	int nbbands;
	float band[MAX_BAND]; /* average per bands */
	float prev[MAX_BAND]; /* prev value per bands */
	float smooth[MAX_BAND]; /* smooth variation of value per bands */
	float delta[MAX_BAND]; /* delta between prev and average value */
	float average, preva, smootha, deltaa; /* idem but for all bands */
} variation_t;


struct light {
	int nb;
	vec3 pos[MAX_LIGHT]; /* vec3 */
	vec3 dir[MAX_LIGHT];
	vec3 ambient; 	/* ambient light color */
	vec3 clr[MAX_LIGHT]; /* color of a light */
	float shininess;
	float strength;
} light_t;


struct hypnomix {
	int x;
	int y;
	int w;
	int h;
	mat4 pmatrix;
	float *samples;
	float *morphsamples;
	float samplesinc;
	struct light lights;
	int nbsamples;
	char *home;
	struct program pg;
	struct preset pr;
	struct transform tr;
	struct module *mod;
	struct variation var;
	char *songname;
	struct dir modlist;
	struct dir prlist;
	void (*log)(int type, const char *frm, ...);
} hypnomix_t;


void hypnomixInit(struct hypnomix *hyp);
void hypnomixRegisterLogFunc(struct hypnomix *hyp, 
	void (*func)(int type, const char *fmt, ...));
void hypnomixDestroy(struct hypnomix *hyp);
void hypnomixLoadShader(struct hypnomix *hyp);
void hypnomixLoadPreset(struct hypnomix *hyp, int id);
void hypnomixMorphSamples(struct hypnomix *hyp);
void hypnomixComputeVariations(struct hypnomix *hyp);
void hypnomixDraw(struct hypnomix *hyp);


#endif

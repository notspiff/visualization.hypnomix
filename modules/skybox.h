#ifndef __SKYBOX_H__
#define __SKYBOX_H__


#include "hypnomix.h"


void skyboxInit(struct hypnomix *hyp);
void skyboxDestroy(struct hypnomix *hyp);
void skyboxDraw(struct hypnomix *hyp, float ax, float ay, float az);


#endif

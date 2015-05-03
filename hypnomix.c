/*
 * Copyright (C) 2014 amand.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "hypnomix.h"
#include "mathematics.h"
#include "module.h"
#include "presets.h"
#include "dir.h"


void hypnomixInit(struct hypnomix *hyp)
{
	int i;
	int err;

	mat4Perspective(hyp->pmatrix, 45.0f, (float)hyp->w / (float)hyp->h, 
		0.1f, 100.0f);

	err = dirListing(&hyp->prlist, hyp->home, ".hpr");
	if(err == -1) {
		(*hyp->log)(HYP_ERROR, "init(): dirListing() .hpr\n");
	}

	err = dirListing(&hyp->modlist, hyp->home, ".so");  
	if(err == -1) {
		(*hyp->log)(HYP_ERROR, "init(): dirListing() .so\n");
	}

	hyp->var.refreshfq = MOVEMENT_FREQ; 
	hyp->var.refresh = 0; /* we start to compute the movement */

	hyp->samplesinc = 0.01; /* FIXME: put this in presets ? */
	hyp->morphsamples = malloc(sizeof(float)*hyp->nbsamples);
	for(i = 0; i < hyp->nbsamples; i++) {	
		hyp->samples[i] = 0.0;	/* reset all samples */
		hyp->morphsamples[i] = 0.0;
	}

	hyp->var.nbbands = 3; /* default is 3 bands */

	hyp->samplesinc = 0.01; /* FIXME: make this variable */

	(*hyp->log)(HYP_SUCCESS, "hypnomixInit()");
}


void hypnomixLoadPreset(struct hypnomix *hyp, int id)
{
	int i = 0;
	struct list *prli = hyp->prlist.li;
	char *filename;

	while(i != id && prli != NULL) {
		i++;
		prli = prli->prev;
	}

	if(prli != NULL) {
		filename = malloc(strlen(hyp->home)+strlen(prli->filename)+1);
		strcpy(filename, hyp->home);
		strcat(filename, prli->filename);
		presetInit(&hyp->pr, filename);
		free(filename);
	} else {
		(*hyp->log)(HYP_WARNING, "No presets found\n");
	}


fprintf(stderr, "MODNAME=%s\n", hyp->pr.modname);

	hyp->mod = NULL;
	if(hyp->pr.modname != NULL) {
		filename = malloc(strlen(hyp->home)+strlen(hyp->pr.modname)+1);
		strcpy(filename, hyp->home);
		strcat(filename, hyp->pr.modname);
		hyp->mod = moduleLoad(hyp, filename);
		if(hyp->mod == NULL) {
			(*hyp->log)(HYP_WARNING, "Module %s not found\n", 
				hyp->pr.modname);
		}
		free(filename);
		hypnomixLoadShader(hyp);
	} 

	/* presetsInitRandomPreset(&hyp->pr);
hyp->pr.idvtxshader = 1; 
hyp->pr.idfrgshader = 1; */
	if(hyp->mod != NULL) {
		(*(hyp->mod->init))(hyp);
	}
	if(hyp->mod != NULL) {
		(*hyp->log)(HYP_INFO, "Module name: %s\n", hyp->mod->name);
	}

	for(i = 0; i < MAX_BAND; i++) { /* reset the bands values */
		hyp->var.band[i] = 0.0;
		hyp->var.prev[i] = 0.0;
		hyp->var.smooth[i] = 0.0;
		hyp->var.delta[i] = 0.0;
	}
	hyp->var.average = 0.0;
	hyp->var.preva = 0.0;
	hyp->var.smootha = 0.0;
	hyp->var.deltaa = 0.0;
}	


void hypnomixRegisterLogFunc(struct hypnomix *hyp, 
	void (*func)(int type, const char *fmt, ...))
{
	hyp->log = func;
}


void hypnomixDestroy(struct hypnomix *hyp)
{
	if(hyp->mod != NULL) {
		(*(hyp->mod->destroy))(hyp);
		moduleUnload(hyp->mod);
	}
	shaderDestroy(hyp->pg.id);
	presetDestroy(&hyp->pr);

	listDestroy(hyp->modlist.li);
	listDestroy(hyp->prlist.li);

	free(hyp->morphsamples);
	(*hyp->log)(HYP_SUCCESS, "hypnomixDestroy()\n");
}


void hypnomixLoadShader(struct hypnomix *hyp)
{
	char *vshfilename = NULL;
	char *fshfilename = NULL;
	char *geofilename = NULL;
	int err;
	const unsigned char *shaderversion;
	int len;

	shaderversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	(*hyp->log)(HYP_INFO, "OpenGL shader version: %s\n", shaderversion);

	len = strlen(hyp->home) + 1;
	if(hyp->pr.vtxshader != NULL) {
		vshfilename = malloc(strlen(hyp->pr.vtxshader) + len);
		strcpy(vshfilename, hyp->home);
		strcat(vshfilename, hyp->pr.vtxshader);
	}
	if(hyp->pr.frgshader != NULL) {
		fshfilename = malloc(strlen(hyp->pr.frgshader) + len);
		strcpy(fshfilename, hyp->home);
		strcat(fshfilename, hyp->pr.frgshader);
	}
	if(hyp->pr.geoshader != NULL) {
		geofilename = malloc(strlen(hyp->pr.geoshader) + len);
		strcpy(geofilename, hyp->home);
		strcat(geofilename, hyp->pr.geoshader);
	}
	err = shaderFileInit(&(hyp->pg.id), vshfilename, 
		fshfilename, geofilename); 

	free(vshfilename);
	free(fshfilename);
	free(geofilename);

	if(err != -1) {
		hyp->pg.mvp = shaderGetUniform(hyp->pg.id, "mvp");
		hyp->pg.mnormal = shaderGetUniform(hyp->pg.id, "mnormal");
		hyp->pg.lightdir = shaderGetUniform(hyp->pg.id, "lightdir");
		hyp->pg.lightpos = shaderGetUniform(hyp->pg.id, "lightpos");
		hyp->pg.shininess = shaderGetUniform(hyp->pg.id, "shininess");
		hyp->pg.strength = shaderGetUniform(hyp->pg.id, "strength");
		hyp->pg.lightclr = shaderGetUniform(hyp->pg.id, "lightclr");
	
		hyp->pg.nblights = shaderGetUniform(hyp->pg.id, "nblights");
		hyp->pg.ambient = shaderGetUniform(hyp->pg.id, "ambient");
		hyp->pg.pos = shaderGetAttribute(hyp->pg.id, "pos");
		hyp->pg.clr = shaderGetAttribute(hyp->pg.id, "clr");
		hyp->pg.nrm = shaderGetAttribute(hyp->pg.id, "nrm");
	} else {
		hyp->mod = NULL;
	}
}


void hypnomixMorphSamples(struct hypnomix *hyp) {
	int i;
//fprintf(stderr, "++++ MorphSamples ++++\n");
	for(i = 0; i < hyp->nbsamples; i++) {
		if(hyp->morphsamples[i] > hyp->samples[i]) {
			hyp->morphsamples[i] -= hyp->samplesinc;
		} else {
			hyp->morphsamples[i] += hyp->samplesinc;
		}
//fprintf(stderr, "%f ", hyp->morphsamples[i]);
	}
//fprintf(stderr, "++++ MorphSamples ++++\n");
}


void hypnomixComputeVariations(struct hypnomix *hyp)
{
	int i, j;
	int nbbands = hyp->var.nbbands;
	int len = hyp->nbsamples / nbbands;

//fprintf(stderr, "<< nbbands = %d, len = %d\n", nbbands, len);

	hyp->var.refresh--;
	if(hyp->var.refresh <= 0) { /* compute new values */
		for(j = 0; j < nbbands; j++) {
			hyp->var.band[j] = 0.0;
		}
		for(i = 0; i < len; i++) {
			for(j = 0; j < nbbands; j++) {
if(hyp->samples[i+j*len] > hyp->var.band[j]) { /* get max value */
	hyp->var.band[j] = hyp->samples[i+j*len];
}
//				hyp->var.band[j] += hyp->samples[i+j*len];
			}
		}
		hyp->var.average = 0.0;
		for(j = 0; j < nbbands; j++) {
// hyp->var.band[j] /= len; /* FIXME: data between 0 and 1 */
			hyp->var.average += hyp->var.band[j];
			if(hyp->var.prev[j] != 0.0) {
				hyp->var.delta[j] = 
					hyp->var.band[j] / hyp->var.prev[j];
			}
			hyp->var.prev[j] = hyp->var.band[j];
		}	
		hyp->var.average /= nbbands;
		if(hyp->var.preva != 0.0) {
			hyp->var.deltaa = hyp->var.average / hyp->var.preva;
		}
		hyp->var.preva = hyp->var.average;
		hyp->var.refresh = hyp->var.refreshfq;
	}
	for(j = 0; j < nbbands; j++) { /* compute smooth values */
// fprintf(stderr, ">> %d) speed = %f, band = %f, smooth = %f\n", j, hyp->pr.speed, hyp->var.band[j], hyp->var.smooth[j]);

		if(hyp->var.band[j] > hyp->var.smooth[j]) {
			hyp->var.smooth[j] += hyp->pr.speed;
		} else {
			hyp->var.smooth[j] -= hyp->pr.speed;
		}
	}
//	speed = hyp->var.deltaa / (hyp->var.refreshfq * 10.0);
	if(hyp->var.average > hyp->var.smootha) {
		hyp->var.smootha += hyp->pr.speed;
	} else {
		hyp->var.smootha -= hyp->pr.speed;
	}
//fprintf(stderr, ">> speed = %f, average = %f, smooth = %f\n", speed, hyp->var.average, hyp->var.smootha);

//for(j = 0; j < hyp->var.nbbands; j++) {
//	fprintf(stderr, "<< smooth[%d] = %f\n", j, hyp->var.smooth[j]);
//}
}


void hypnomixDraw(struct hypnomix *hyp)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	hyp->lights.nb = 1;

	vec3Set(hyp->lights.pos[0], 0.0, 0.0, 1.0); 
	vec3Set(hyp->lights.dir[0], 0.0, 0.0, -1.0);
	vec3Set(hyp->lights.clr[0], 1.0, 1.0, 1.0); 
	vec3Set(hyp->lights.ambient, 0.25, 0.25, 0.25); 
	hyp->lights.shininess = 2.0;
	hyp->lights.strength = 0.6;

	glUseProgram(hyp->pg.id);

	glUniform3fv(hyp->pg.lightpos, hyp->lights.nb, hyp->lights.pos[0]);
	glUniform3fv(hyp->pg.lightdir, hyp->lights.nb, hyp->lights.dir[0]);
	glUniform3fv(hyp->pg.lightclr, hyp->lights.nb, hyp->lights.clr[0]);
	glUniform1i(hyp->pg.nblights, hyp->lights.nb);
	glUniform3fv(hyp->pg.ambient, 1, hyp->lights.ambient);
	glUniform1f(hyp->pg.strength, hyp->lights.strength);
	glUniform1f(hyp->pg.shininess, hyp->lights.shininess);
	if(hyp->mod != NULL) {
		(*(hyp->mod->draw))(hyp);
	} 

	glDisable(GL_DEPTH_TEST);
}

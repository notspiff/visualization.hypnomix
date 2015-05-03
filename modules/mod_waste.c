#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hypnomix.h"
#include "particles.h"
#include "texture.h"



char modname[] = "waste";
static struct particle_system ps;

GLuint particletex[1];

char particleimg[] = "particle.png";
static int nbsources;
static vec4 clrbck;


void init(struct hypnomix *hyp)
{
	int i;
	char *path;
	float tmpf;
	char *texname;

	transformProjection(&hyp->tr, hyp->pmatrix);
	hyp->var.nbbands = 20;
	if(presetGetStr(&hyp->pr, "texture", &texname) == -1) {
		texname = malloc(strlen(particleimg));
		strcpy(texname, particleimg);
	};

	textureInit(particletex, 1);

	path = malloc(strlen(hyp->home) + strlen(texname) + 1);
	strcpy(path, hyp->home);
	strcat(path, texname);
	textureLoadWithFilename(path, &particletex[0]);
	free(path);

	if(presetGetFloat(&hyp->pr, "nbsources", 1, &tmpf, 
		NULL, NULL, NULL) == 1) {
		nbsources = tmpf;
	} else {
		nbsources = 1;
	}

	if(presetGetFloat(&hyp->pr, "clrbck", 4, &clrbck[0], &clrbck[1],
		&clrbck[2], &clrbck[3]) != 4) {
		vec4Set(clrbck, 0.0, 0.0, 0.0, 1.0);
	} 

	particlesInit(&ps, 5000*nbsources, nbsources, 3);
	ps.maxx = 2.0;
	ps.minx = -2.0;
	ps.maxy = 2.0;
	ps.miny = -2.0;
	ps.maxz = 2.0;
	ps.minz = -2.0;

	for(i = 0; i < nbsources; i++) {
		vec3Set(ps.emit[i].loc, 0.0, 0.0, -1.0);
		vec3Set(ps.emit[i].dir, -0.00001, 0.0, 0.0);
		vec4Copy(hyp->pr.clr0, ps.emit[i].clr); 
		ps.emit[i].life = 1000;
		ps.emit[i].freq = 1;
		ps.emit[i].time = ps.emit[i].freq;
		ps.emit[i].mass = 0.5*i;
	}

	vec3Set(ps.forces[0].loc, 0.0, 0.5, -1.5);
	ps.forces[0].type = PARTICLE_FORCE_ATTRACTION;
	ps.forces[0].value = 10.0;

	vec3Set(ps.forces[1].loc, 0.0, -0.5, 0.5);
	ps.forces[1].type = PARTICLE_FORCE_ATTRACTION;
	ps.forces[1].value = 20.0;

	ps.forces[2].type = PARTICLE_FORCE_DIRECTIONAL;
	vec3Set(ps.forces[2].dir, 0.0, -0.0001, 0.0);
	ps.forces[2].value = 2.0; 
}


void destroy(struct hypnomix *hyp)
{
	particlesDestroy(&ps);
	textureDestroy(particletex, 1);
	fprintf(stderr, "INFO: module destroy\n");
}


void draw(struct hypnomix *hyp)
{
	int stride = sizeof(struct particle); 
	int i;

	transformIdentity(&hyp->tr);
	// transformModelRotate(&hyp->tr, 0.0, a, 0.0);
	transformViewTranslate(&hyp->tr, 0.0, 0.0, -4.0);
	transformMVP(&hyp->tr);

	glUseProgram(hyp->pg.id);

	glEnableVertexAttribArray(hyp->pg.clr);
	glEnableVertexAttribArray(hyp->pg.pos);
//	glEnableVertexAttribArray(hyp->pg.texpos);

	glUniformMatrix4fv(hyp->pg.mvp, 1, GL_FALSE, hyp->tr.mvp);

// fprintf(stderr, "%f\n", hyp->var.average);
	for(i = 0; i < hyp->var.nbbands; i++) {
		ps.emit[i].loc[0] = (hyp->var.average-0.1) * 2.0;
		ps.emit[i].loc[1] = (hyp->var.average-0.1) * 2.0;
		ps.emit[i].loc[2] = (hyp->var.average-0.1) * 2.0;

// fprintf(stderr, "%f ", ps.emit[i].loc[1]);
		ps.emit[i].mass = hyp->var.smooth[i];
		ps.emit[i].clr[0] = hyp->var.smooth[i];
		ps.emit[i].clr[1] = cos(hyp->var.smooth[i]);
		ps.emit[i].clr[2] = hyp->var.smooth[i];
	} 
		
	for(i = 0; i < 2; i++) {
		ps.forces[i].value = 40.0 * (i+1.0) * hyp->var.average;
	}

	particlesMove(&ps);

#ifdef __APPLE__
	glEnable(GL_PROGRAM_POINT_SIZE_EXT);
#else
	glEnable(GL_PROGRAM_POINT_SIZE);
#endif

	glEnable(GL_POINT_SPRITE); 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(clrbck[0], clrbck[1], clrbck[2], clrbck[3]);
	glClear(GL_COLOR_BUFFER_BIT);

//glBindTexture(GL_TEXTURE_2D, particletex[0]);
//glActiveTexture(GL_TEXTURE0);
//glUniform1i(hyp->pg.texture0, 0);

	glVertexAttribPointer(hyp->pg.pos, 3, GL_FLOAT, GL_FALSE, stride, 
		(GLvoid *)ps.particles.pool);
//glVertexAttribPointer(hyp->pg.texpos, 2, GL_FLOAT, GL_FALSE, stride, 
//(GLvoid *)ps.particles.pool+sizeof(vec3));

	glVertexAttribPointer(hyp->pg.clr, 4, GL_FLOAT, GL_FALSE, stride, 
		(GLvoid *)ps.particles.pool+sizeof(vec3)+sizeof(vec2));
	glDrawArrays(GL_POINTS, 0, ps.particles.id);

	glDisable(GL_BLEND);
	glDisable(GL_POINT_SPRITE);

// glDisableVertexAttribArray(hyp->pg.texpos);
	glDisableVertexAttribArray(hyp->pg.clr);
	glDisableVertexAttribArray(hyp->pg.pos); 
}

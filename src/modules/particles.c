#include <stdio.h>
#include <stdlib.h>

#include "pool.h"
#include "particles.h"


/* http://natureofcode.com/book/ */


struct pool pool;


void particlesInit(struct particle_system *ps, int nbparticles, int nbemitters,
	int nbforces)
{
	poolInit(&ps->particles, nbparticles, sizeof(struct particle));
	ps->nbemitters = nbemitters;
	if(nbemitters > 0) {
		ps->emit = malloc(sizeof(struct particle_emitter)*nbemitters);	
	} else {
		ps->emit = NULL;
	}
	ps->nbforces = nbforces;
	if(nbforces > 0) {
		ps->forces = malloc(sizeof(struct particle_force)*nbforces);	
	} else {
		ps->forces = NULL;
	}
}


void particlesDestroy(struct particle_system *ps)
{
	free(ps->emit);
	free(ps->forces);
	poolDestroy(&ps->particles);
}


void particlesDump(struct pool *pool)
{
	int i;
	struct particle *p;

	fprintf(stderr, "NB=%d\n", pool->nbitems);
	for(i = 0; i < pool->id; i++) {
		p = (struct particle*)(pool->pool) + i;
		fprintf(stderr, "P(%d) %f,%f,%f\n", i, 
			p->loc[0], p->loc[1], p->loc[2]);
	}
}


static void applyForces(struct particle_system *ps, struct particle *p)
{
	int i = 0; 
	vec3 force;
	vec3 dir;
	float m, d, s;
	
	vec3Copy(p->dir, p->acc);
	do {
		switch(ps->forces[i].type) {
			case PARTICLE_FORCE_DIRECTIONAL:
				vec3Copy(ps->forces[i].dir, force);
				break;
			case PARTICLE_FORCE_FLUID:
				s = vec3Mag(p->acc);
				vec3Copy(p->acc, force);
				vec3Normalize(force, force);
				vec3Mul(force, -ps->forces[i].value*s*s, force);
				break;
			case PARTICLE_FORCE_ATTRACTION:
				vec3Sub(ps->forces[i].loc, p->loc, dir);
				d = vec3Mag(dir);
				vec3Normalize(dir, dir);
				m = 0.00001*ps->forces[i].value*p->mass / (d*d);
				vec3Mul(dir, m, force);				
				break;
		}
		vec3Add(p->acc, force, p->acc);
	} while(++i < ps->nbforces);
	vec3Div(p->acc, p->mass, p->acc);
	vec3Add(p->vel, p->acc, p->vel);
	vec3Add(p->loc, p->vel, p->loc);
}


static void add(struct particle_emitter *emit, struct pool *pool)
{
	struct particle p;

	vec3Copy(emit->loc, p.loc);
	vec3Copy(emit->dir, p.dir);
	p.tex[0] = 0.0;
	p.tex[1] = 0.0;
	vec4Copy(emit->clr, p.clr);
// vec4Set(p.clr, 1.0, 2.0, 3.0, 4.0);
	vec3Set(p.acc, 0.0, 0.0, 0.0);
	vec3Set(p.vel, 0.0, 0.0, 0.0);
	p.life = emit->life;
	p.mass = emit->mass;

	poolAdd(pool, &p);
/*
fprintf(stderr, "EMIT %d\n", pool->id);
particlesDump(pool); */
}


void particlesMove(struct particle_system *ps)
{
	int i;
	int nbparticles = ps->particles.id;
	struct particle *p;

	i = 0;
	while(nbparticles < ps->particles.nbitems && i < ps->nbemitters) {
		if(--ps->emit[i].time <= 0) {
			add(ps->emit+i, &ps->particles);	
			ps->emit[i].time = ps->emit[i].freq;
		}
		i++;
	}
	
	for(i = 0; i < nbparticles; i++) {
		p = (struct particle*)(ps->particles.pool) + i;
		applyForces(ps, p);
		if((--p->life < 0)
			|| p->loc[0] > ps->maxx || p->loc[0] < ps->minx
			|| p->loc[1] > ps->maxy || p->loc[1] < ps->miny
			|| p->loc[2] > ps->maxz || p->loc[2] < ps->minz) {
			poolDelete(&ps->particles, i); 
		} 
			
	}  
	poolFlush(&ps->particles);
}

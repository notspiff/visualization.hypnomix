#ifndef __PARTICLES_H__
#define __PARTICLES_H__


#include "pool.h"
#include "mathematics.h"


#define PARTICLE_FORCE_DIRECTIONAL 1
#define PARTICLE_FORCE_FLUID 2
#define PARTICLE_FORCE_ATTRACTION 3


struct particle {
	vec3 loc; /* location */
	vec2 tex;
	vec4 clr;
	vec3 dir; /* direction of the emitter */
	vec3 acc; /* acceleration */
	vec3 vel; /* velocity */
	int life;
	float mass;
} particle_t;


struct particle_emitter {
	vec3 loc;
	vec4 clr;
	vec3 dir;
	int life;
	float mass;
	int freq;
	int time;
} particle_emitter_t;


struct particle_force {
	vec3 loc;
	vec3 dir;
	float value;
	int type;
} particle_force_t;


struct particle_system {
	struct pool particles;
	struct particle_emitter *emit;
	int nbemitters;
	struct particle_force *forces;
	int nbforces;
	float minx, maxx;
	float miny, maxy;
	float minz, maxz;
} particle_system_t;


void particlesInit(struct particle_system *ps, int nbparticles, 
	int nbemitters, int nbforces);
void particlesDestroy(struct particle_system *ps);
void particlesMove(struct particle_system *ps);


#endif

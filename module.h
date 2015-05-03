#ifndef __MODULE_H__
#define __MODULE_H__


#include "shader.h"
#include "presets.h"
#include "transform.h"
#include "hypnomix.h"


struct module {
	char *name;
	void (*init)(struct hypnomix *hyp);
	void (*destroy)(struct hypnomix *hyp);
	void (*draw)(struct hypnomix *hyp);
	void *lib;
} module_t;


struct module *moduleLoad(struct hypnomix *hyp, const char *filename);
void moduleUnload(struct module *mod);


#endif

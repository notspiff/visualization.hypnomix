#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "module.h"


struct module *moduleLoad(struct hypnomix *hyp, const char *filename)
{
	char *name;
	struct module *mod;

fprintf(stderr, "TRY TO LOAD MODULE\n");

	mod = malloc(sizeof(struct module));
	if((mod->lib = dlopen(filename, RTLD_LAZY)) == NULL) {
		fprintf(stderr, "ERROR: module %s not found...\n", name);
		free(mod);
		return NULL;
	}
	if((name = dlsym(mod->lib, "modname")) == NULL) {
		fprintf(stderr, "ERROR: modname not found in module %s...\n",
			name);
		free(mod);
		return NULL;
	}
	mod->name = malloc(strlen(name)+1);
	strcpy(mod->name, name);
	if((mod->init = dlsym(mod->lib, "init")) == NULL) {
		fprintf(stderr, "ERROR: init() not found in module %s...\n",
			name);
		free(mod);
		return NULL;
	}
	if((mod->destroy = dlsym(mod->lib, "destroy")) == NULL) {
		fprintf(stderr, "ERROR: destroy() not found in module %s...\n",				name);
		free(mod);
		return NULL;
	}
	if((mod->draw = dlsym(mod->lib, "draw")) == NULL) {
		fprintf(stderr, "ERROR: draw() not found in module %s...\n",
			name);
		free(mod);
		return NULL;
	}
fprintf(stderr, "MODULE LOAD\n");
	return mod;
}


void moduleUnload(struct module *mod)
{
	if(mod != NULL) {
		if(dlclose(mod->lib) == -1) {
			fprintf(stderr, "ERROR: error closing module %s...\n",
				mod->name);
		}
		free(mod->name);
		free(mod);
	}
}

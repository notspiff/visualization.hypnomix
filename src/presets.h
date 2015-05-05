#ifndef __PRESETS_H__
#define __PRESETS_H__


struct preset_dict {
	char *key;
	char *value;
} preset_dict_t;


struct preset {
	struct preset_dict *dict;
	int nb;
	char *name;
	char *modname;
	char *frgshader;
	char *vtxshader;
	char *geoshader;
	float speed;
	float var[3];
	float clr0[4];
	int glslminor, glslmajor;
} preset;


int presetGetStr(struct preset *pr, const char *key, char **value);
int presetGetFloat(struct preset *pr, char *key, int nb, float *n1, float *n2, 
	float *n3, float *n4);
void presetDump(struct preset *pr);
int presetInit(struct preset *pr, const char *filename);
void presetDestroy(struct preset *pr);


#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "presets.h"


static char *presetReadFile(const char *filename)
{
	FILE *fp;
	char *buf;
	struct stat st;

	fp = fopen(filename, "r");
	if(fp == NULL) { 
		return NULL;
	} 	
	stat(filename, &st);
	buf = malloc(st.st_size+1);
	if(fread(buf, 1, st.st_size, fp) < st.st_size) {
		return NULL;
	}
	fclose(fp);
	buf[st.st_size] = '\0';

	return buf;
}


static int presetParse(struct preset *pr, char *buf)
{
	char *line;
	int nbassign = 0;
	int len;
	int i;
	int pos;

	i = 0;
	while(buf[i]) {
		if(buf[i++] == '=') {
			nbassign++;
		}
	}
	pr->dict = malloc(sizeof(struct preset_dict)*nbassign);
	pr->nb = 0;
	while((line = strsep(&buf, "\n")) != NULL) {
		len = strlen(line);
		pos = 0;
		while(line[pos] != '=' && pos < len) {
			pos++;
		}
		if(pos < len) {
			pr->dict[pr->nb].key = strndup(line, pos);
			pr->dict[pr->nb].value = strndup(line+pos+1, len-pos-1);
			pr->nb++;
		}
	}
	return pr->nb;
}


int presetGetStr(struct preset *pr, const char *key, char **value)
{
	int i = 0;

	*value = NULL;
	while(i < pr->nb) {
		if(!strcmp(pr->dict[i].key, key)) {
			*value = pr->dict[i].value;
		}	
		i++;
	}
	if(i < pr->nb) {
		return 0;
	} else {
		return -1;
	}
}


int presetGetFloat(struct preset *pr, char *key, int nb, float *n1, float *n2, 
	float *n3, float *n4) 
{
	int i;

	if(nb > 4) { 
		nb = 4;
	} 	
	i = 0;
	while(i < pr->nb) {
		if(!strcmp(pr->dict[i].key, key)) {
			return sscanf(pr->dict[i].value, "%f %f %f %f",
				n1, n2, n3, n4);
		}
		i++;
	} 
	return 0;
}


void presetDump(struct preset *pr)
{
	int i;

	for(i = 0; i < pr->nb; i++) {
		printf("DUMP: (%s)=(%s)\n", pr->dict[i].key, pr->dict[i].value);
	}
}


int presetInit(struct preset *pr, const char *filename)
{
	char *buf;
	char *version;

	buf = presetReadFile(filename);
	if(buf != NULL) {
		presetParse(pr, buf);
		free(buf);

		/* init default values */
		presetGetStr(pr, "name", &pr->name);
		presetGetStr(pr, "module", &pr->modname);
		presetGetStr(pr, "vtxshader", &pr->vtxshader);
		presetGetStr(pr, "frgshader", &pr->frgshader);
		pr->geoshader = NULL;
		presetGetStr(pr, "geoshader", &pr->geoshader);
		presetGetStr(pr, "glslversion", &version);	
		if(sscanf(version, "%d.%d", &pr->glslmajor, 
			&pr->glslminor) != 2) {
			pr->glslmajor = 1; /* default version is 1.2 */
			pr->glslminor = 2;
		}
		if(presetGetFloat(pr, "speed", 1, &pr->speed, NULL, NULL, 
			NULL) != 1) {
			pr->speed = 0;
		}
		if(presetGetFloat(pr, "var", 3, &pr->var[0], &pr->var[1],
			&pr->var[2], NULL) != 3) {		
			bzero(pr->var, sizeof(float)*3);
		}
		if(presetGetFloat(pr, "clr0", 4, &pr->clr0[0], &pr->clr0[1],
			&pr->clr0[2], &pr->clr0[3]) != 4) {
			bzero(pr->clr0, sizeof(float)*4);
		}
		return 0;
	} 
	return -1;
}


void presetDestroy(struct preset *pr)
{
	int i;

	for(i = 0; i < pr->nb; i++) {
		free(pr->dict[i].key);
		free(pr->dict[i].value);
	}
	if(pr->dict != NULL) {
		free(pr->dict);
	}
}

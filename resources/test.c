#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>


struct preset {	
	char *name;
	char *modname;
	char *vtxshader;
	char *frgshader;
	float varx, vary, varz;
} preset_t;


static void parseString(char *line, const char *token, char **dst) {
	char tmpstr[256]; /* FIXME: max string 256 chars */

	if(sscanf(line, token, tmpstr)) {
		*dst = malloc(strlen(tmpstr)+1);
		bzero(*dst, strlen(tmpstr)+1);
		strncpy(*dst, tmpstr, strlen(tmpstr));
	} 
}


void presetRead(struct preset *pr, const char *filename)
{
	FILE *fp;
	struct stat statinfo;
	long len;
	char *line;
	char *buf;
	char tmpstr[256]; /* FIXME: max string size = 256 */
	float tmpf;

	fp = fopen(filename, "r");
	if(fp == NULL) {
		fprintf(stderr, "presetRead(): %s not found...\n", filename);
		exit(EXIT_FAILURE);
	}
	stat(filename, &statinfo);
	len = statinfo.st_size;
	buf = malloc(len);
	fread(buf, 1, len, fp);
	fclose(fp);

	pr->name = NULL;
	pr->modname = NULL;
	pr->vtxshader = NULL;
	pr->frgshader = NULL;
	
	line = strtok(buf, "\n");	
	while(line) {
		printf("LINE=%s\n", line);
		parseString(line, "name=%s\n", &pr->name);
		parseString(line, "module=%s\n", &pr->modname);
		parseString(line, "vtxshader=%s\n", &pr->vtxshader);
		parseString(line, "frgshader=%s\n", &pr->frgshader);
		sscanf(line, "varx=%f\n", &pr->varx);
		sscanf(line, "vary=%f\n", &pr->vary);
		sscanf(line, "varz=%f\n", &pr->varz);
		line = strtok(NULL, "\n");
	}
	free(buf);

	printf("%s\n%s\n%s\n", pr->modname, pr->vtxshader, pr->frgshader);
	printf("%f,%f,%f\n", pr->varx, pr->vary, pr->varz);
}


int main(int argc, char *argv[])
{
	struct preset pr;

	presetRead(&pr, "basic.hpr");

	return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hypnomix.h"
#include "texture.h"
#include "font.h"


#define FONT_NAME "kimberley.ttf"


char modname[] = "flag";


void init(struct hypnomix *hyp)
{
	char *fontpath;

	hyp->var.nbbands = 3;

//	textureInit();

	fontpath = malloc(strlen(hyp->home)+strlen(FONT_NAME)+1);
	strcpy(fontpath, hyp->home);
	strcat(fontpath, FONT_NAME);
	if(fontInit(hyp, fontpath, 32, 32) == -1) {
		hyp->mod = NULL;
	};
	free(fontpath);
}


void destroy(struct hypnomix *hyp)
{
	fontDestroy();
//	textureDestroy();
}


void draw(struct hypnomix *hyp)
{
	fontPrint(hyp, hyp->songname, hyp->var.smooth[0], hyp->var.smooth[1], 
		0xff, 0xff, 0xff, 0xff);
}

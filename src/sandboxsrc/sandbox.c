#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>

#include "x11gl.h"
#include "ticktimer.h"
#include "hypnomix.h"

#define SHADERPATH "../visualization.hypnomix/resources/"

static struct ticktimer timer;
static struct hypnomix hyp;

#define NB_SAMPLES 256

static int presetid;

static int var = 0;
void animate(void)
{
	double ms = ticktimerCheck(&timer);
	int i;

	if(ms > TICKTIMER_60FPS) {
		for(i = 0; i < NB_SAMPLES; i++) {
			hyp.samples[i] = fabs(cosf((var+i)*M_PI/(NB_SAMPLES*2.0)));
		}
		var++;
		hypnomixMorphSamples(&hyp);
		hypnomixComputeVariations(&hyp);

		hypnomixDraw(&hyp);
		X11GLSwapBuffers();
		ticktimerStart(&timer);
	}	
}


void keypress(unsigned int keycode)
{
	if(keycode == K_ESC) {
		x11glquit = 1;
	}
	if(keycode == K_SPACE) {
		hypnomixLoadPreset(&hyp, presetid);
	}
}


void cleanExit(int sig)
{
	x11glquit = 1;
}


void printLog(int type, const char *fmt, ...)
{
	va_list vl;

	switch(type) {
		case HYP_ERROR:
			printf("[HYPNOMIX ERROR] ");
			break;
		case HYP_WARNING:
			printf("[HYPNOMIX WARNING] ");
			break;
		case HYP_INFO:
			printf("[HYPNOMIX INFO] ");
			break;
		case HYP_DEBUG:
			printf("[HYPNOMIX DEBUG] ");
			break;
		case HYP_SUCCESS:
			printf("[HYPNOMIX SUCCESS] ");
			break;
	} 
	
	va_start(vl, fmt);
	vprintf(fmt, vl); /* FIXME: use CLog::Log */
	va_end(vl);
}

int main(int argc, char *argv[])
{
	if(argc > 1) {
		presetid = atoi(argv[1]);
	} else {
		presetid = 0;
	}
	hyp.x = 100;
	hyp.y = 100;
	hyp.w = 640;
	hyp.h = 480;
	hyp.home = malloc(strlen(SHADERPATH)+1);	
	hyp.samples = malloc(sizeof(float)*NB_SAMPLES);
	hyp.nbsamples = NB_SAMPLES;
	strcpy(hyp.home, SHADERPATH);
	hyp.songname = malloc(strlen("hypnomix")+1);
	strcpy(hyp.songname, "hypnomix");

	X11GLInit(hyp.x, hyp.y, hyp.w, hyp.h);
	X11RegisterKeypress(&keypress);
	hypnomixRegisterLogFunc(&hyp, printLog);
	hypnomixInit(&hyp);
	hypnomixLoadPreset(&hyp, presetid);

	signal(SIGINT, cleanExit);

	ticktimerStart(&timer);
	while(!x11glquit) {
		animate();
		X11GLEvent();
	}

	free(hyp.songname);
	free(hyp.samples);
	free(hyp.home);
	hypnomixDestroy(&hyp);
	X11GLDestroy();
	printf("INFO: clean exit...\n");

	return EXIT_SUCCESS;
}

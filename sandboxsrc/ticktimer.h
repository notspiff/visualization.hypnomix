#ifndef __TICKTIMER_H__
#define __TICKTIMER_H__


#include <sys/time.h>

#define TICKTIMER_60FPS 16.667 /* 1000.0 / 60.0 */
#define TICKTIMER_120FPS 8.333 /* 1000.0 / 120.0 */


typedef struct ticktimer {
	struct timeval start;
	struct timeval stop;
	double milliseconds;
} ticktimer_s;


void ticktimerStart(struct ticktimer *timer);
double ticktimerCheck(struct ticktimer *timer);
void ticktimerStop(struct ticktimer *timer);


#endif

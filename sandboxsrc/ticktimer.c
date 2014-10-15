#include <stdlib.h>
#include <sys/time.h>

#include "ticktimer.h"


void ticktimerStart(struct ticktimer *timer) 
{
	gettimeofday(&timer->start, NULL);
	timer->stop = timer->start;
}


double ticktimerCheck(struct ticktimer *timer) 
{
	double msstart, msstop;

	gettimeofday(&timer->stop, NULL);
	msstart = timer->start.tv_sec * 1000.0L 
		+ (double)timer->start.tv_usec / 1000.0L;
	msstop = timer->stop.tv_sec * 1000.0L 
		+ (double)timer->stop.tv_usec / 1000.0L;
	timer->milliseconds = msstop - msstart;

	return timer->milliseconds;
}


void ticktimerStop(struct ticktimer *timer) 
{
	timer->milliseconds = 0;
}

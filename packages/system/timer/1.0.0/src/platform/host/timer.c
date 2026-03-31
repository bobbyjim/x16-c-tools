#include <stdio.h>
#include <time.h>

#include "timer.h"

struct timespec ts;

void pause_jiffies(unsigned jiffies) {
	printf("[timer stub] pause_jiffies(%u)\n", jiffies);
}

long getMillis()
{
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_nsec / 1000000;
}

#include "core.h"
#include "clock.h"

#include <sys/time.h>

int64_t hires_time()
{
	int64_t t;
	timeval tv;
	gettimeofday(&tv, NULL);
	t = tv.tv_sec * 1000000;
	t += tv.tv_usec;
	return t;
}

int64_t hires_frequency()
{
	return 1000000ll;
}
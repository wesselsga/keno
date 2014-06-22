#include "core.h"
#include "clock.h"


int64_t hires_time()
{
	LARGE_INTEGER qpc;
	::QueryPerformanceCounter(&qpc);	
	return qpc.QuadPart;
}

int64_t hires_frequency()
{
	LARGE_INTEGER qpc;
	::QueryPerformanceFrequency(&qpc);	
	return qpc.QuadPart;
}
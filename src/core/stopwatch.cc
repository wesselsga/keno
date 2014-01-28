#include "core.h"
#include "stopwatch.h"

#ifndef _WIN32
#include <sys/time.h>
#endif

int64_t hires_time()
{
	int64_t t;
#ifdef _WIN32	
	LARGE_INTEGER qpc;
	::QueryPerformanceCounter(&qpc);	
	t = qpc.QuadPart;
#else
	timeval tv;
	gettimeofday(&tv, NULL);
	t = t1.tv_sec * 1000000;
	t += t1.tv_usec;
#endif	
	return t;
}

int64_t hires_frequency()
{
#ifdef _WIN32
	LARGE_INTEGER qpc;
	::QueryPerformanceFrequency(&qpc);	
	return qpc.QuadPart;
#else
	return 1000000ll;
#endif
}

//
//
// this time span class is based on the concept of ticks and a tick
// is measured in 100-ns intervals or 1 millisecond = 10000 ticks
//
const int64_t __ticksPerMillisecond = 1000 * 10;
const int64_t __ticksPerSecond      = __ticksPerMillisecond * 1000;
const int64_t __ticksPerMinute      = __ticksPerSecond * 60;
const int64_t __ticksPerHour        = __ticksPerMinute * 60;
const int64_t __ticksPerDay         = __ticksPerHour * 24;
const int64_t __minMilliseconds     = -922337203685477L;
const int64_t __maxMilliseconds     = 922337203685477L;

/*TimeSpan::TimeSpan() : _ticks(0L)
{
}

TimeSpan::TimeSpan(int64_t ticks)
{
	_ticks = ticks;
}

TimeSpan::TimeSpan(int32_t hours, 
			int32_t mins, int32_t seconds, int32_t milliseconds)
{
	int64_t ms = (((hours * 3600L) + 
				(mins * 60L) + (seconds)) * 1000L) + milliseconds;

	_ticks = ms_to_ticks(ms * 1.0);
}

TimeSpan::TimeSpan(const TimeSpan& other)
{
	assign(other);
}

TimeSpan::TimeSpan(double seconds)
{
	_ticks = ms_to_ticks(seconds * 1000.0);
}

TimeSpan::~TimeSpan()
{
}

int32_t TimeSpan::ticks_to_ms32(int64_t ticks) 
{ 
	return static_cast<int32_t>(ticks / __ticksPerMillisecond);
}

double TimeSpan::ticks_to_secs(int64_t ticks) 
{ 
	return ticks / (__ticksPerSecond * 1.0);
}

double TimeSpan::ticks_to_ms(int64_t ticks) 
{ 
	return ticks / (__ticksPerMillisecond * 1.0);
}

int32_t TimeSpan::days() const
{
	int32_t n = static_cast<int32_t>(_ticks / double(__ticksPerDay));
	return n;
}

int32_t TimeSpan::hours() const
{
	int32_t n = static_cast<int32_t>((_ticks / __ticksPerHour) % 24L);
	return n;
}

int32_t TimeSpan::minutes() const
{
	int32_t n = static_cast<int32_t>((_ticks / __ticksPerMinute) % 60L);
	return n;
}

int32_t TimeSpan::seconds() const
{
	int32_t s = static_cast<int32_t>((_ticks / __ticksPerSecond) % 60L);
	return s;
}

double TimeSpan::milliseconds() const
{
	int64_t remain = (_ticks % __ticksPerSecond);	
	return (remain / (__ticksPerMillisecond * 1.0));
}

double TimeSpan::total_seconds() const
{
	return (_ticks / (__ticksPerSecond * 1.0));
}

double TimeSpan::total_milliseconds() const
{
	return (_ticks / (__ticksPerMillisecond * 1.0));
}

void TimeSpan::assign(const TimeSpan& other)
{
	_ticks = other._ticks;
}

int64_t TimeSpan::ms_to_ticks(double ms)
{
	//double t = ms + ((ms >= 0.0) ? 0.5 : -0.5);
	double t = ms;// + ((ms >= 0.0) ? 0.05 : -0.05);

	if ( (t > __maxMilliseconds) || (t < __minMilliseconds) )
	{
		throw std::bad_exception("time span is too long");
	}
	
	return static_cast<int64_t>(t * __ticksPerMillisecond);
}

std::ostream& operator<<(std::ostream& out, const TimeSpan& span)
{
	int64_t tick = span.ticks();

	int32_t d = static_cast<int32_t>(tick / double(__ticksPerDay));
	int64_t remain = tick % __ticksPerDay;

	if (tick < 0L)
	{
		d = d * -1;
		remain = remain * -1;
	}
	
	int32_t h = static_cast<int32_t>((remain / __ticksPerHour) % 24L);
	int32_t m = static_cast<int32_t>((remain / __ticksPerMinute) % 60L);
   int32_t s = static_cast<int32_t>((remain / __ticksPerSecond) % 60L);

	remain = (remain % __ticksPerSecond);	
	float ms = static_cast<float>(remain / (__ticksPerMillisecond * 1.0));
	
	if (tick < 0L)
	{
		out << "-";	
	}
	
	if (d == 0)
	{
		out << std::setfill('0') 
			<< std::setw(2) << h << ":"
			<< std::setw(2) << m << ":"
			<< std::setw(2) << s << ":"
			<<  std::setw(6) << std::fixed << std::setprecision(2) << ms;
	}
	else
	{
		out << std::setfill('0') 
			<< std::setw(2) << d << ":"
			<< std::setw(2) << h << ":"
			<< std::setw(2) << m << ":"
			<< std::setw(2) << s << ":"
			<< std::setw(6) << std::fixed << std::setprecision(2) << ms;
	}

	return out;
}
*/


// static member for clock frequency
/*StopWatch::StopWatch()
{
	init(0);   
}

StopWatch::StopWatch(int64_t time)
{
	init(time);
}

StopWatch::~StopWatch()
{
}

void StopWatch::init(int64_t time)
{
	_begin = 0;
	_time = time;
	_started = false;

	_freq = hires_frequency();
}

// Marks the start time for measuring elapsed time
//
void StopWatch::start()
{
	if (!_started)
	{
		_time = 0;
		_begin = hires_time();
		_started = true;
	}
}

void StopWatch::stop()
{
	if (_started)
	{
		//ASSERT(_freq > 0);
		_time = (hires_time() - _begin);	
		_started = false;
	}
}

void StopWatch::set(int64_t time)
{
	_started = false;
	_begin = 0;
	_time = time;
}

TimeSpan StopWatch::elapsed() const
{
	int64_t t;
	if (_started)
	{
		// if still running, grab counter
		t = (hires_time() - _begin);
	}
	else
	{
		t = _time;
	}

	// calculate the elapsed time in seconds
	double secs = ((double)(t) / (double)_freq);
	
	return TimeSpan(secs);
}

std::ostream& operator<<(std::ostream& out, const StopWatch& sw)
{
	out << sw.elapsed();
	return out;
}*/
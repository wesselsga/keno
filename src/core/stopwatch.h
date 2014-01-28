#pragma once

//
// various time and clock utilities
//
//

int64_t hires_time();
int64_t hires_frequency();

//
// uses 100-ns ticks
//
/*class TimeSpan
{
public:
   TimeSpan();
	TimeSpan(const TimeSpan& other);
	TimeSpan(int64_t ticks);
	TimeSpan(int32_t hours, int32_t mins, int32_t seconds, int32_t milliseconds);
	TimeSpan(double seconds);

   ~TimeSpan();

	// from ticks to seconds or milliseconds
	static double ticks_to_secs(int64_t ticks);
	static double ticks_to_ms(int64_t ticks);
	static int32_t ticks_to_ms32(int64_t ticks);

	// from seconds or milliseconds to ticks
	static int64_t secs_to_ticks(double s){ return ms_to_ticks(s* 1000.0); }
	static int64_t ms_to_ticks(double ms);
	
	int32_t days() const;
	int32_t hours() const;
	int32_t minutes() const;
	int32_t seconds() const;
	double milliseconds() const;

	double total_seconds() const;
	double total_milliseconds() const;

	int64_t ticks() const { return _ticks; }
	
private:

	void assign(const TimeSpan& other);

	int64_t _ticks;

};*/

//std::ostream& operator<<(std::ostream&, const TimeSpan&);

/*class StopWatch
{
public:
   StopWatch();
	StopWatch(int64_t time);

	~StopWatch();

   void start();
   void stop();
	
	void reset(){ return set(0); }

	void set(int64_t time);
	
	TimeSpan elapsed() const;

	bool is_started() const { return _started; }
	
private:

   int64_t _freq;
   
	void init(int64_t time);

	int64_t _begin;
	int64_t _time;
	bool _started;

};*/

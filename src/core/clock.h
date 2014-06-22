#pragma once

//
// various time and clock utilities
//
//

extern int64_t hires_time();
extern int64_t hires_frequency();

class Clock
{
public:
   virtual ~Clock(){}

   static std::shared_ptr<Clock> create();

   virtual int64_t ticks() const=0;
   virtual int64_t frequency() const=0;

protected:
   Clock(){}

};
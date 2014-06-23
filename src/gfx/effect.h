#pragma once

namespace gfx {


typedef float (*Easing)(float,float,float,float);

class Effect
{
public:
   virtual ~Effect(){}

   static std::shared_ptr<Effect> create(
            std::string const&,
            uint32_t,
            std::shared_ptr<Clock> const&);

   uint32_t duration() const { return _duration; }

   void select(std::shared_ptr<Layer> const&);
   
   virtual void reset();
   virtual bool animate()=0;

protected:
   explicit Effect(std::shared_ptr<Clock> const& clock, uint32_t duration) 
      : _clock(clock), _start(0), _duration(duration)
   {
   }

   float elapsed() const;
   
   std::shared_ptr<Clock> const _clock;
   std::vector<std::shared_ptr<Layer>> _layers;
   int64_t _start;
   uint32_t const _duration;

};

class Fade : public Effect
{
public:
   explicit Fade(
            std::shared_ptr<Clock> const&, 
            uint32_t, 
            bool, 
            Easing);

   void reset();   
   bool animate();

private:

   Easing    _easing;
   bool _inverse;
};


};

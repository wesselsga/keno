#include "gfx.h"
#include "effect.h"
#include <clock.h>

namespace tween {

   float linear(float t, float b, float c, float d)
   {
      return ((c * t) / d) + b;
   }

   float easeOutQuad(float t, float b, float c, float d)
   {
      t = t / d;
      return -c * t * (t-2) + b;
   }
   
   float easeInOut(float t, float b, float c, float d)
   {
	   t = t / d;
      return c * (t * t + b);	   
   }


} // tween


using namespace gfx;


std::shared_ptr<Effect> Effect::create(
      std::string const& which, 
      uint32_t duration, 
      std::shared_ptr<Clock> const& clock)
{
   std::shared_ptr<Effect> fx;

   if (which == "fadein"){
      fx = std::make_shared<Fade>(clock,duration,false,tween::easeOutQuad);
   }
   else if (which == "fadeout"){
      fx = std::make_shared<Fade>(clock,duration,true,tween::easeOutQuad);
   }


   return fx;
}


void Effect::select(std::shared_ptr<Layer> const& layer)
{
   if (layer){
      _layers.push_back(layer);
   }
}

void Effect::reset()
{
   _start = _clock ? _clock->ticks() : 0ll;
}

float Effect::elapsed() const
{
   if (!_clock){
      return 0.0f;
   }

   double f = static_cast<double>(_clock->frequency());
   double t = (_clock->ticks() - _start) / f;
   
   return static_cast<float>(t * 1000.0f);
}

Fade::Fade(std::shared_ptr<Clock> const& clock, 
   uint32_t duration,
   bool inverse, Easing easing) 
   : Effect(clock, duration), 
   _inverse(inverse), 
   _easing(easing)
{
}

void Fade::reset()
{
   for (auto layer : _layers) 
   {
      layer->setOpacity(_inverse ? 1.0f : 0.0f);
      layer->setVisible(true);
   }
   Effect::reset();
}

bool Fade::animate()
{
   float t = static_cast<float>(elapsed());

   float opacity = _easing(t, 0.0, 255.0f, _duration);

   opacity = opacity / 255.0f;

   //LOG(VERBOSE) << "fx: clock=" << t << ", opacity=" << opacity;
   
   for (auto l : _layers) {
      l->setOpacity(_inverse ? 1.0f - opacity : opacity);
   } 

   return (t < static_cast<float>(_duration));
}



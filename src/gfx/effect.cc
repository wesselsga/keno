#include "gfx.h"
#include "effect.h"
#include <clock.h>

namespace tween {

   float linear(float t, float b, float c, float d)
   {
      return ((c * t) / d) + b;
   }

} // tween


using namespace gfx;


std::shared_ptr<Effect> Effect::create(std::string const& which, 
      std::shared_ptr<Clock> const& clock)
{
   std::shared_ptr<Effect> fx;

   if (which == "fadein"){
      fx = std::make_shared<FadeIn>(clock);
   }
   else if (which == "fadeout"){
      fx = std::make_shared<FadeOut>(clock);
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

void FadeIn::reset()
{
   for (auto l : _layers) {
      l->setOpacity(0.0f);
   }
   Effect::reset();
}

bool FadeIn::animate()
{
   float t = static_cast<float>(elapsed());

   float opacity = tween::linear(t, 0.0, 255.0f, 1000.0f);

   opacity = opacity / 255.0f;

   for (auto l : _layers) {
      l->setOpacity(opacity);
   } 

   return true;
}

void FadeOut::reset()
{
   for (auto l : _layers) {
      l->setOpacity(1.0f);
   }
   Effect::reset();
}

bool FadeOut::animate()
{
   float t = static_cast<float>(elapsed());

   float opacity = tween::linear(t, 0.0, 255.0f, 1000.0f);
   opacity = opacity / 255.0f;

   for (auto l : _layers) {
      l->setOpacity(1.0f - opacity);
   } 

   return true;
}


float easeInOut(float t, float b, float c, float d)
{
	/*t = t / (d / 2);

	if (t < 1){
		return (c / 2) * t * t + b;
	}

	--t;

	return -c / 2 * (t * (t-2) - 1) + b;*/

	t = t / d;

	return c * (t * t + b);
	//return -c * t * (t-2) + b;
}
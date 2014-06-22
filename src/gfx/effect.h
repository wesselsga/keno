#pragma once

namespace gfx {

class Effect
{
public:
   virtual ~Effect(){}

   static std::shared_ptr<Effect> create(
            std::string const&, 
            std::shared_ptr<Clock> const&);

   void select(std::shared_ptr<Layer> const&);
   
   virtual void reset();
   virtual bool animate()=0;

protected:
   explicit Effect(std::shared_ptr<Clock> const& clock) 
      : _clock(clock), _start(0)
   {
   }

   float elapsed() const;
   
   std::shared_ptr<Clock> const _clock;
   std::vector<std::shared_ptr<Layer>> _layers;
   int64_t _start;

};

class FadeIn : public Effect
{
public:
   explicit FadeIn(std::shared_ptr<Clock> const& clock) 
      : Effect(clock) 
   {
   }

   void reset();   
   bool animate();

private:

};

class FadeOut : public Effect
{
public:
   explicit FadeOut(std::shared_ptr<Clock> const& clock) 
      : Effect(clock) 
   {
   }

   void reset();
   bool animate();

private:

};


};

#pragma once

namespace gfx {

class Layer;
class Program;

class Effect;

class Compositor
{
public:
	explicit Compositor();
	virtual ~Compositor();

   void process();

   std::shared_ptr<Layer> layer(size_t const) const;

   std::shared_ptr<Layer> addLayer(const std::shared_ptr<Stream>&);

   std::shared_ptr<Effect> addEffect(std::string const&, size_t);

private:
	Compositor(const Compositor&)=delete;
	const Compositor& operator=(const Compositor&) = delete;
	
   void initBicubic();
   void initLUT(uint32_t, uint32_t);

   void createKernel(int32_t, float, float);

   std::vector<std::shared_ptr<Layer>> _layers;

   std::vector<std::shared_ptr<Effect>> _effects;

   std::shared_ptr<Program> _program;

};


class Effect
{
public:
   virtual ~Effect(){}

   void select(std::shared_ptr<Layer> const&);
   
   virtual void reset()=0;
   virtual bool animate()=0;

protected:
   Effect(){}

   std::vector<std::shared_ptr<Layer>> _layers;

};

class FadeIn : public Effect
{
public:
   FadeIn() : _step(0) { }

   void reset();   
   bool animate();

private:

   uint16_t _step;

};

class FadeOut : public Effect
{
public:
   FadeOut() : _step(0) { }

   void reset();
   bool animate();

private:

   uint16_t _step;
};





};

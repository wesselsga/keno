#pragma once

class Clock;

namespace gfx {

class Layer;
class Effect;
class Program;

class Compositor
{
public:
	explicit Compositor(std::shared_ptr<Clock> const&);
	virtual ~Compositor();

   void process();

   std::shared_ptr<Layer> layer(size_t const) const;

   std::shared_ptr<Layer> addLayer(const std::shared_ptr<Stream>&);

   std::shared_ptr<Effect> addEffect(std::string const&, uint32_t, size_t);
   std::shared_ptr<Effect> addEffect(std::string const&, std::string const&, size_t);

private:
	Compositor(const Compositor&)=delete;
	const Compositor& operator=(const Compositor&) = delete;
	
   void initBicubic();
   void initLUT(uint32_t, uint32_t);

   void createKernel(int32_t, float, float);

   std::vector<std::shared_ptr<Layer>> _layers;

   std::vector<std::shared_ptr<Effect>> _effects;

   std::shared_ptr<Program> _program;

   std::shared_ptr<Clock> _clock;

};


};

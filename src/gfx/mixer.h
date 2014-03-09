#pragma once

namespace gfx {

class Layer;
class Program;

class Mixer
{
public:
	explicit Mixer();
	virtual ~Mixer();

   void process();

   void addLayer(const std::shared_ptr<Stream>&);

private:
	Mixer(const Mixer&)=delete;
	const Mixer& operator=(const Mixer&) = delete;
	
   void initBicubic();
   void initLUT(uint32_t, uint32_t);

   void createKernel(int32_t, float, float);

   std::vector<std::shared_ptr<Layer>> _layers;

   std::shared_ptr<Program> _program;

};

};

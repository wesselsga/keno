#pragma once

namespace gfx {

class Layer;
class Program;

class Compositor
{
public:
	explicit Compositor();
	virtual ~Compositor();

   void process();

   std::shared_ptr<Layer> layer(size_t const) const;

   std::shared_ptr<Layer> addLayer(const std::shared_ptr<Stream>&);

private:
	Compositor(const Compositor&)=delete;
	const Compositor& operator=(const Compositor&) = delete;
	
   void initBicubic();
   void initLUT(uint32_t, uint32_t);

   void createKernel(int32_t, float, float);

   std::vector<std::shared_ptr<Layer>> _layers;

   std::shared_ptr<Program> _program;

};

};

#pragma once

#include <atomic>

namespace gfx {

class Layer
{
	struct _priv { };
public:
	explicit Layer(const _priv&, const std::shared_ptr<Stream>&);
	virtual ~Layer();

	static std::shared_ptr<Layer> create(
            const std::string&, 
            const std::shared_ptr<Stream>&,
            float zorder);

   uint32_t texture() const { return _stream->texture(); }
   uint32_t model() const { return _model; }

   float opacity() const;
   void setOpacity(float const);

   bool visible() const { return _visible; }
   void setVisible(bool const);

private:
	Layer()=delete;
	Layer(const Layer&)=delete;
	const Layer& operator=(const Layer&) = delete;

   std::shared_ptr<Stream> _stream;

   uint32_t _model;
   std::atomic_bool _visible;
   float    _opacity;
};

};

#pragma once

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

private:
	Layer()=delete;
	Layer(const Layer&)=delete;
	const Layer& operator=(const Layer&) = delete;

   std::shared_ptr<Stream> _stream;

   uint32_t _model;

};

};

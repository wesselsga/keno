#pragma once

namespace gfx {

class Image : public Stream
{
	struct _priv { };
public:
	explicit Image(const _priv&, uint32_t, uint32_t, uint8_t*);
	virtual ~Image();

	static std::shared_ptr<Image> create(const std::string&);
   
   uint32_t width() const { return _width; }
   uint32_t height() const { return _height; }  
   uint32_t texture();

private:
	Image()=delete;
	Image(const Image&)=delete;
	const Image& operator=(const Image&) = delete;
	
   const uint32_t _width;
   const uint32_t _height;
   uint8_t* _pixmap;
   uint32_t _tex;
};

};

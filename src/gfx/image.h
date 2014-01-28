#pragma once

namespace gfx {

class Image
{
	struct _ctx { };
public:
	explicit Image(const _ctx&);
	virtual ~Image(){}

	static std::shared_ptr<Image> create(const std::string&);

private:
	Image()=delete;
	Image(const Image&)=delete;
	const Image& operator=(const Image&) = delete;
	
};

};

#include "gfx.h"

#include <FreeImage.h>

using namespace gfx;

Image::Image(const _ctx&)
{

}


std::shared_ptr<Image> Image::create(const std::string& filename)
{
	FREE_IMAGE_FORMAT fmt = FreeImage_GetFileType(filename.c_str(), 0);
	
	if (fmt == FIF_UNKNOWN) {
		fmt = FreeImage_GetFIFFromFilename(filename.c_str() );
	}
	
	// if still unkown, return failure.
	if (fmt == FIF_UNKNOWN) 
	{
		LOG(ERR) << "image: could not determine format: " << filename;
		return nullptr;
	}

	FIBITMAP* bitmap = FreeImage_Load(fmt, filename.c_str());
	if (!bitmap)
	{
		LOG(ERR) << "image: could not load image: " << filename;
		return nullptr;
	}
	
	auto img(std::make_shared<Image>(_ctx{}));

	/*std::shared_ptr<Image> img;

	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	int color_type, interlace_type;
	
	FILE *fp;
	if ((fp = fopen(filename.c_str(), "rb")) == NULL){
		return false;
	}

	png_ptr = png_create_read_struct(
		PNG_LIBPNG_VER_STRING,
		NULL, 
		NULL, 
		NULL);
	
	if (!png_ptr) {
		fclose(fp);
		return false;
	}

	// Allocate/initialize the memory
	// for image information.  REQUIRED.
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) 
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return false;
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	fclose(fp);*/
	
	return img;	
}
#include "gfx.h"

#include <FreeImage.h>

#ifdef _RASPI
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif


using namespace gfx;

Image::Image(const _priv&, uint32_t w, uint32_t h, uint8_t* pixmap) 
   : _pixmap(pixmap), _width(w), _height(h)
{
   _tex = 0;
}

Image::~Image()
{
   free(_pixmap);

   if (_tex){
      glDeleteTextures(1, &_tex);
   }
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

	FIBITMAP* bmp = FreeImage_Load(fmt, filename.c_str());
	if (!bmp)
	{
		LOG(ERR) << "image: could not load image: " << filename;
		return nullptr;
	}

   uint32_t w = FreeImage_GetWidth(bmp);
   uint32_t h = FreeImage_GetHeight(bmp);
   if (!w || !h)
   {
		LOG(ERR) << "image: invalid dimension: " << filename;
		FreeImage_Unload(bmp);
      return nullptr;
	}

   bool alpha = true;

   FIBITMAP* temp = bmp;
	bmp = FreeImage_ConvertTo32Bits(temp);
	FreeImage_Unload(temp);
   
   uint8_t* pixmap = reinterpret_cast<uint8_t*>(malloc(w * h * 4));
   if (!pixmap)
   {
      FreeImage_Unload(bmp);
      return nullptr;
   }

   //FreeImage_ConvertToRawBits(pixmap, bmp, w*4, 32, 0, 0, 0);

   uint8_t* pixels = reinterpret_cast<uint8_t*>(FreeImage_GetBits(bmp)); 
   for (int j= 0; j < w*h; ++j)
   {
		pixmap[j*4+0] = pixels[j*4+2];
		pixmap[j*4+1] = pixels[j*4+1];
		pixmap[j*4+2] = pixels[j*4+0];
		pixmap[j*4+3] = pixels[j*4+3];
	}

   FreeImage_Unload(bmp);
   	
	auto img(std::make_shared<Image>(_priv{}, w, h, pixmap));

   GLenum err;

   GLuint tex;	
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			alpha ? GL_RGBA : GL_RGB, 
			w,
			h, 
			0, 
			alpha ? GL_RGBA : GL_RGB, 
			GL_UNSIGNED_BYTE,
			pixmap);		

	err = glGetError();
   LOG_IF(WARN, (err!=GL_NO_ERROR)) << "error initializing texture";

   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);   
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   
   err = glGetError();
   LOG_IF(WARN, err!=GL_NO_ERROR) << "error in texture";

	glBindTexture(GL_TEXTURE_2D, 0);

   img->_tex = tex;
	
	return img;	
}

uint32_t Image::texture()
{
   return _tex;
}

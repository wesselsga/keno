#include "gfx.h"


#ifdef _RASPI
#include <GLES2/gl2.h>
#else
#include <GL/glew.h>
#endif

using namespace gfx;

struct vertex
{
	GLfloat tex[2];
	GLfloat color[4];
	GLfloat pos[4];
};

Layer::Layer(const _priv&, const std::shared_ptr<Stream>& stream)
   : _stream(stream)
{
   _visible = true;
   _model = 0;
   _opacity = 1.0f;
}

Layer::~Layer()
{
   glDeleteBuffers(1, &_model);
}

std::shared_ptr<Layer> Layer::create(
      const std::string&, const std::shared_ptr<Stream>& stream, float zorder)
{
   auto layer = std::make_shared<Layer>(_priv{}, stream);

   glGenBuffers(1, &layer->_model);
	glBindBuffer(GL_ARRAY_BUFFER, layer->_model);

   // get current viewport
	GLint vp[4];
   glGetIntegerv(GL_VIEWPORT, vp);

	GLfloat x0 = static_cast<GLfloat>(0.0f);
	GLfloat x1 = static_cast<GLfloat>(0.0f + vp[2]);
	GLfloat y0 = static_cast<GLfloat>(0.0f);
	GLfloat y1 = static_cast<GLfloat>(0.0f + vp[3]);
   
   struct vertex quad[] = {                    /*x*/ /*y*/  /*z*/
		{ {0.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {x0,   y0,   zorder, 1.0f} },
		{ {1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {x1,   y0,   zorder, 1.0f} },
		{ {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {x0,   y1,   zorder, 1.0f} },
		{ {1.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {x1,   y1,   zorder, 1.0f} }
	};

	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(struct vertex), quad, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return layer;
}

float Layer::opacity() const
{
   return _opacity;
}

void Layer::setOpacity(float const val)
{
   _opacity = val;
}

void Layer::setVisible(bool const flag)
{
   _visible = flag;
}

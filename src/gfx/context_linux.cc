#include "gfx.h"

#include <GL/glew.h>
#include <GL/glxew.h>
#include "context_linux.h"

using namespace gfx;

GlxContext::GlxContext()
{
   _ctx = nullptr;
}

GlxContext::~GlxContext()
{
}

std::shared_ptr<Context> GlxContext::create(
      void* native_display, void* native_window)
{
	return nullptr;
}

void GlxContext::bind()
{
   glXMakeCurrent(_display, _window, _ctx);
}

void GlxContext::unbind()
{
   glXMakeCurrent(0, 0, 0);
}

void GlxContext::clear()
{
	glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT*/);
}

void GlxContext::swapBuffers()
{
   glXSwapBuffers(_display, _window);
}

void GlxContext::finish()
{
	glFinish();
}

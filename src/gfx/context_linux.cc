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

std::shared_ptr<Context> GlxContext::create(void* native_display)
{
	return nullptr;
}

void GlxContext::bind()
{
}

void GlxContext::unbind()
{
}

void GlxContext::clear()
{
	glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT*/);
}

void GlxContext::swapBuffers()
{
}

void GlxContext::finish()
{
	glFinish();
}

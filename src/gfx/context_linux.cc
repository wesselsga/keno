#include "gfx.h"

#include "context_linux.h"

#include <GL/glew.h>

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
	wglMakeCurrent(_hdc, _hrc);
}

void GlxContext::unbind()
{
	wglMakeCurrent(NULL, NULL);
}

void GlxContext::clear()
{
	glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT*/);
}

void GlxContext::swapBuffers()
{
	::SwapBuffers(_hdc);
}

void GlxContext::finish()
{
	glFinish();
}
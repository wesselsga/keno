#include "gfx.h"

#ifdef _WIN32
#include "context_win.h"
#else
#include "context_egl.h"
#endif

using namespace gfx;

std::shared_ptr<Context> Context::create(
         void* native_display, void* native_window)
{
	std::shared_ptr<Context> ctx;
	
#ifdef _WIN32
	ctx = WglContext::create(native_window);
#else
#ifdef _RASPI
	ctx = EglContext::create(native_window);
#else
	ctx = GlxContext::create(native_display, native_window);
#endif
#endif

	return ctx;
}
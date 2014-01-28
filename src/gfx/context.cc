#include "gfx.h"

#ifdef _WIN32
#include "context_win.h"
#else
#include "context_egl.h"
#endif

using namespace gfx;

std::shared_ptr<Context> Context::create(void* native_display)
{
	std::shared_ptr<Context> ctx;
	
#ifdef _WIN32
	ctx = WglContext::create(native_display);
#else
	ctx = EglContext::create(native_display);
#endif

	return ctx;
}
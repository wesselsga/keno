#include "gfx.h"

#include "context_win.h"

#include <GL/glew.h>
#include <GL/wglew.h>

using namespace gfx;

WglContext::WglContext() : _hrc(0), _hdc(0)
{
}

WglContext::~WglContext()
{
}

std::shared_ptr<Context> WglContext::create(void* native_display)
{
	std::shared_ptr<Context> ctx;

	HWND hwnd = (HWND)native_display;
	if (!::IsWindow(hwnd)){
		return ctx;
	}

	HDC hdc = ::GetDC(hwnd);
	
	PIXELFORMATDESCRIPTOR pfd ;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1; 
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;  
	pfd.iPixelType = PFD_TYPE_RGBA; 
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32; 
	pfd.iLayerType = PFD_MAIN_PLANE;
	 
	int32_t format = ChoosePixelFormat(hdc, &pfd);
	if (format == 0) 
	{
		::ReleaseDC(hwnd, hdc);
		return ctx; 
	}
	
	if (!::SetPixelFormat (hdc, format, &pfd)) 
	{
		::ReleaseDC(hwnd, hdc);
		return ctx;
	}
	
	// --- OpenGL 3.x ---
   HGLRC tmp = wglCreateContext(hdc); 
   wglMakeCurrent(hdc, tmp);

	GLenum err = glewInit();
	if (err != GLEW_OK) 
	{
		::ReleaseDC(hwnd, hdc);
		return ctx;
	}
 
	int32_t attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		WGL_CONTEXT_FLAGS_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_ES2_PROFILE_BIT_EXT,
		0
	};

	HGLRC hrc = 0; 
	
	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		hrc = wglCreateContextAttribsARB(hdc, 0, attribs);
		
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tmp);
		wglMakeCurrent(hdc, hrc);
	}
	else
	{	
		hrc = tmp;
	}
 
	//Checking GL version
	const unsigned char* sver = glGetString(GL_VERSION);

	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

	WglContext* wctx = new WglContext();
	wctx->_hrc = hrc;
	wctx->_hdc = hdc;

	/*if (wglewIsSupported("WGL_EXT_swap_control_tear") == 1)
	{
		wglSwapIntervalEXT(-1);		
	}
	else if (wglewIsSupported("WGL_EXT_swap_control") == 1)
	{
		wglSwapIntervalEXT(1);		
	}*/	

	ctx.reset(wctx);

	LOG(VERBOSE) << "wgl: context \'" << sver << "\'";
	
	return ctx;
}

void WglContext::bind()
{
	wglMakeCurrent(_hdc, _hrc);
}

void WglContext::unbind()
{
	wglMakeCurrent(NULL, NULL);
}

void WglContext::clear()
{
	glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT*/);
}

void WglContext::swapBuffers()
{
	::SwapBuffers(_hdc);
}

void WglContext::finish()
{
	glFinish();
}
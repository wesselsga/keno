#include "gfx.h"

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include "context_egl.h"

using namespace gfx;

EglContext::EglContext()
{
	_ctx = nullptr;
	_display = nullptr;
	_surface = nullptr;
}

EglContext::~EglContext()
{
}

std::shared_ptr<Context> EglContext::create(void* handle)
{
	std::shared_ptr<EglContext> ctx;

	EGLint numConfigs;
   EGLint major;
   EGLint minor;
   EGLDisplay display;
   EGLContext context;
   EGLSurface surface;
   EGLConfig config;
   
   display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   if (display == EGL_NO_DISPLAY) {
      return ctx;
   }

   // Initialize EGL
   if ( !eglInitialize(display, &major, &minor) )
   {
      return ctx;
   }

	LOG(INFO) << "egl: initialized version " << major << "." << minor;

   // get configs
   if (!eglGetConfigs(display, NULL, 0, &numConfigs))
   {
      return ctx;
   }

	EGLint attribs[] =
   {
       EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
       EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
       EGL_BUFFER_SIZE,     32,
       EGL_RED_SIZE,        8,
       EGL_GREEN_SIZE,      8,
       EGL_BLUE_SIZE,       8,
       EGL_ALPHA_SIZE,      8,
       EGL_NONE
   };

   // Choose config
   if ( !eglChooseConfig(display, attribs, &config, 1, &numConfigs) )
   {
      return ctx;
   }

	// bind the OpenGL API to the EGL
   if (!eglBindAPI(EGL_OPENGL_ES_API))
	{
		LOG(ERR) << "egl: error binding api";
		return ctx;
	}

	// Create a GL context
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };   
   context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
   if ( context == EGL_NO_CONTEXT )
   {
      return ctx;
   }

   // Create a surface
   surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)handle, NULL);
   if ( surface == EGL_NO_SURFACE )
   {
      return ctx;
   }  
   
   // Make the context current
   if ( !eglMakeCurrent(display, surface, surface, context) )
   {
      return ctx;
   }

	ctx.reset(new EglContext());

	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClearColor(1.0f, 0.27f, 0.20f, 1.0f);
   
	ctx->_ctx = context;
	ctx->_display = display;
	ctx->_surface = surface;
   	
	return ctx;
}

void EglContext::bind()
{
	//wglMakeCurrent(_hdc, _hrc);
}

void EglContext::unbind()
{
	//wglMakeCurrent(NULL, NULL);
}

void EglContext::clear()
{
	glClear(GL_COLOR_BUFFER_BIT/* | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT*/);
}

void EglContext::swapBuffers()
{
	eglSwapBuffers(_display, _surface);
}

void EglContext::finish()
{
	//glFinish();
}

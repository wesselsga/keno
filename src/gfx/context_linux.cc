#include "gfx.h"

#include "context_linux.h"

using namespace gfx;

GlxContext::GlxContext()
{
   _ctx = nullptr;
}

GlxContext::~GlxContext()
{
   if (_display && _ctx){
      glXDestroyContext(_display, _ctx);
   }
}

std::shared_ptr<Context> GlxContext::create(
      void* native_display, void* native_window)
{
   Display* display = reinterpret_cast<Display*>(native_display);
   if (!display){ 
      return nullptr; 
   }

   Window win = reinterpret_cast<Window>(native_window);
   
   XWindowAttributes wattr;
   XGetWindowAttributes(display, win, &wattr);

   /*static int32_t attribs[] =
   {
      GLX_X_RENDERABLE,  True,
      GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
      GLX_RENDER_TYPE,   GLX_RGBA_BIT,
      GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
      GLX_RED_SIZE,      8,
      GLX_GREEN_SIZE,    8,
      GLX_BLUE_SIZE,     8,
      GLX_ALPHA_SIZE,    8,
      GLX_DEPTH_SIZE,    24,
      GLX_STENCIL_SIZE,  8,
      GLX_DOUBLEBUFFER,  True,
      None
   };
 
   // FBConfigs were added in GLX version 1.3.
   int32_t major, minor; 
   if (!glXQueryVersion(display, &major, &minor) ||
      ( (_major == 1) && (minor < 3)) || (major < 1))
   {
      LOG(ERR) << "glx: invalid GLX version";
      return nullptr;
   }

   int32_t fbcount;
   GLXFBConfig* fbc = glXChooseFBConfig(display, 
                     DefaultScreen(display), 
                     attribs,
                     &fbcount);
   if (!fbc)
   {
      LOG(ERR) << "glx: failed to retrieve framebuffer config";
      return nullptr;
   }

   GLXFBConfig cfg = fbc[best_fbc];
   XFree(fbc);*/
   
   GLXContext ctx = glXCreateNewContext(display,cfg,GLX_RGBA_TYPE,0,True);
   if (!ctx)
   {
      LOG(ERR) << "glx: error creating default context";
      return nullptr;
   }

   // initialize GLEW
   GLenum err = glewInit();
	if (err != GLEW_OK) 
	{
      LOG(ERR) << "glx: error initializing GLEW";
      
      glXDestroyContext(display, ctx);
		return ctx;
	}
   
   // select the frambuffer with the most samples per pixel
   /*int32)t best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
   for (int32_t n=0; n < fbcount; ++n)
   {
      XVisualInfo *vi = glXGetVisualFromFBConfig( display, fbc[i] );
      if (vi)
      {
         int samp_buf, samples;
         glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf );
         glXGetFBConfigAttrib( display, fbc[i], GLX_SAMPLES       , &samples  );
 
         //printf( "  Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
         //    " SAMPLES = %d\n", 
         //    i, vi -> visualid, samp_buf, samples );
 
         if (best_fbc < 0 || samp_buf && samples > best_num_samp )
         best_fbc = i, best_num_samp = samples;
         if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp )
         worst_fbc = i, worst_num_samp = samples;
      }
      XFree(vi);
   }
 
   GLXFBConfig cfg = f
   bc[best_fbc];
   XFree(fbc);*/


   int32_t ctx_attribs[] =
	{
		GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		GLX_CONTEXT_FLAGS_ARB, 0,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
		0
	};

	if (glxewIsSupported("WGL_ARB_create_context") == 1)
	{
      ctx = glXCreateContextAttribsARB(display, 
                     cfg, 
                     0, True, 
                     ctx_attribs);

		glXMakeCurrent(display, 0);
		glXDestroyContext(display, ctx);
		glXMakeCurrent(display, ctx);
	}
	else
	{
	}

   if (!ctx)
   {
      LOG(ERR) << "glx: could not create context";
      return nullptr;
   }

   return nullptr;
}

void GlxContext::bind()
{
   glXMakeCurrent(_display, _window, _ctx);
}

void GlxContext::unbind()
{
   glXMakeCurrent(_display, 0, 0);
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

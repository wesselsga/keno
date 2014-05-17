#include "gfx.h"

#include "context.h"

#include <unistd.h>


namespace gfx {

class GlxContext : public Context
{
public:
	~GlxContext();

	static std::shared_ptr<Context> create(void*,void*);
	
	void bind();
	void unbind();
	void clear();
	void swapBuffers();
	void finish();

private:
	GlxContext();

   GLXContext _ctx;

   Display* _display;
   GLXDrawable _window;


};

std::shared_ptr<Context> Context::create(
         void* native_display, void* native_window)
{
	return GlxContext::create(native_display, native_window);
}


}; // gfx




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
  
   // the framebuffer config should be in the window hash
   // table under key # 0
   XPointer xptr;
   if (XFindContext(display, win, 0, &xptr))
   {
      LOG(ERR) << "glx: error finding frame buffer config";
      return nullptr;      
   }
   
   GLXFBConfig fbcfg = reinterpret_cast<GLXFBConfig>(xptr);  
   if (!fbcfg)
   {
      LOG(ERR) << "glx: invalid frame buffer config";
      return nullptr;      
   }
   
   auto ctx = std::shared_ptr<GlxContext>(new GlxContext());
   ctx->_display = display;
   ctx->_window = win;   
   
   // we cannot use GLEW yet until we create and bind a context
   auto creatorARB = (GLXContext(*)(Display*, GLXFBConfig, GLXContext, int, const int*))
      glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
         
   int32_t gl_attribs[] =
   {
      GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
      GLX_CONTEXT_MINOR_VERSION_ARB, 0,
      GLX_CONTEXT_FLAGS_ARB, 0,
      GLX_CONTEXT_PROFILE_MASK_ARB, 0x00000004/*GLX_CONTEXT_ES2_PROFILE_BIT_EXT*/,
      0
   };  
      
   if (creatorARB)
   {
      ctx->_ctx = creatorARB(display, fbcfg, 0, True, gl_attribs);
   }
   else
   {
      
   }
   
   auto bindProc = (int(*)(Display*, GLXDrawable, GLXContext))
         glXGetProcAddressARB((const GLubyte*)"glXMakeCurrent");
   if (!bindProc)
   {
      LOG(ERR) << "glx: unable to activate context";
      return nullptr;
   }

   bindProc(display, win, ctx->_ctx);
   
   GLenum err = glewInit();
   if (err != GLEW_OK)
   {
      LOG(ERR) << "glx: error initializing GLEW";
      return nullptr;      
   }
   
   LOG(VERBOSE) << "glx: " << glGetString(GL_VERSION);
   
   glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
   
   return ctx;
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

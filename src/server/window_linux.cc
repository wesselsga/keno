#include "server.h"
#include "channel.h"

#include "window.h"

#include <X11/Xlib.h>

#include <GL/glew.h>
#include <GL/glxew.h>

#include <X11/XKBlib.h>


namespace screen {

class X11Window : public Window
{
	struct _ctx { };
public:
	explicit X11Window(const _ctx&, const std::weak_ptr<Channel>&);
	virtual ~X11Window();

	static std::shared_ptr<X11Window> create(
            const std::string&,
            const uint32_t width,
            const uint32_t height,
            const std::weak_ptr<Channel>&);
   
   void*   handle() const { return reinterpret_cast<void*>(_frame); }
   void*   display() const { return _display; }
	void    show() const;
	void    update() const;
	void    close();

   int32_t pump();

private:

	X11Window()=delete;
	X11Window(const X11Window&)=delete;
	const X11Window& operator=(const X11Window&) = delete;

   Display* _display;
   Visual*  _visual;
   ::Window  _frame;

};

};



std::shared_ptr<screen::Window> screen::Window::create(
         const std::string& title,
         const uint32_t width,
         const uint32_t height,
			const std::weak_ptr<Channel>& channel)
{
   return X11Window::create(title, width, height, channel);
}



using namespace screen;

X11Window::X11Window(const _ctx&, 
      const std::weak_ptr<Channel>& channel)
    : Window(channel) 
{
   _display = nullptr;
   _visual = nullptr;
   _frame = 0;
}

X11Window::~X11Window()
{
	
}

std::shared_ptr<X11Window> X11Window::create(
         const std::string& title,
         const uint32_t width,
         const uint32_t height,
			const std::weak_ptr<Channel>& channel)
{
   std::shared_ptr<X11Window> win(
            std::make_shared<X11Window>(_ctx{}, channel));
	
   LOG(VERBOSE) << "x11: creating native window...";

   win->_display = XOpenDisplay(NULL);
   
   int fb_attribs[] =
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
 
   // not even bothering with anything < 1.3 for GLX
   int32_t major, minor; 
   if (!glXQueryVersion(win->_display, &major, &minor) ||
      ( (major == 1) && (minor < 3)) || (major < 1))
   {
      LOG(ERR) << "x11: invalid GLX version (requires 1.3+)";
      return nullptr;
   }
   
   auto chooseProc = (GLXFBConfig*(*)(Display*, int, const int*, int*))
         glXGetProcAddressARB((const GLubyte*)"glXChooseFBConfig");

   int32_t fbcount;
   GLXFBConfig* fbc = chooseProc(win->_display, 
                     DefaultScreen(win->_display), 
                     fb_attribs,
                     &fbcount);
   if (!fbc)
   {
      LOG(ERR) << "xll: failed to retrieve framebuffer config";
      return nullptr;
   }
   
   GLXFBConfig cfg = fbc[0];
   XFree(fbc);
   
   //GLXFBConfig* cfg = reinterpret_cast<GLXFBConfig*>(malloc(sizeof(GLXFBConfig)));
   //memcpy(cfg, fbc, sizeof(GLXFBConfig));
      
   auto viProc = (XVisualInfo*(*)(Display*, GLXFBConfig))
         glXGetProcAddressARB((const GLubyte*)"glXGetVisualFromFBConfig");
   
   XVisualInfo* vi = viProc(win->_display, cfg);
      
   XSetWindowAttributes win_attribs;
   win_attribs.background_pixmap = None;
   win_attribs.border_pixel = 0;
   win_attribs.event_mask = StructureNotifyMask | KeyPressMask;
   win_attribs.background_pixel = XWhitePixel(win->_display, 0);
   win_attribs.colormap = XCreateColormap(win->_display,
                        RootWindow(win->_display, vi->screen),
                        vi->visual,
                        AllocNone);
    
   // create the application window
   win->_frame = XCreateWindow(win->_display, 
               RootWindow(win->_display, vi->screen),
               0, 0, width, height, 0, 
               vi->depth,
               InputOutput, 
               vi->visual, 
               CWBorderPixel|CWColormap|CWEventMask,
               &win_attribs);
   
   // store the frame buffer config we selected for gl context
   // creation later
   XSaveContext(win->_display, 
                win->_frame, 
                0, 
                XPointer(cfg));   
   
   
   XStoreName(win->_display, win->_frame, title.c_str());
    
   XSelectInput(win->_display, win->_frame, ExposureMask | StructureNotifyMask);
    
   return win;
}

int32_t X11Window::pump()
{
   if (!_display){
      return 0;
   }
   
   XEvent evnt;
   
   while (XPending(_display))
   {
      XNextEvent(_display, &evnt);
      
      switch (evnt.type)
      {
         case KeyPress:
            break;
         default : break;         
      }
   }
   
   
   /*if (XCheckWindowEvent(_display, _frame, KeyPressMask, &evnt))
   {
      char* key = XKeysymToString(
                  XkbKeycodeToKeysym(_display, evnt.xkey.keycode, 0, 0));
      
      LOG(VERBOSE) << "x11: key press " << key;                  
      
   }*/

   return 0;
}

void X11Window::show() const
{
   XMapWindow(_display, _frame);
}

void X11Window::update() const
{
}

void X11Window::close()
{
}

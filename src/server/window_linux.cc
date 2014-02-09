#include "server.h"
#include "channel.h"
#include "window_linux.h"

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
			const std::weak_ptr<Channel>& channel)
{
   std::shared_ptr<X11Window> win(
   std::make_shared<X11Window>(_ctx{}, channel));
	
   LOG(VERBOSE) << "x11: creating native window...";

   int32_t depth;
   XSetWindowAttributes    frame_attributes;
   XFontStruct* fontinfo;
   XGCValues gr_values;
   XKeyEvent event;

   win->_display = XOpenDisplay(NULL);
   win->_visual = DefaultVisual(win->_display, 0);
   depth  = DefaultDepth(win->_display, 0);
    
   frame_attributes.background_pixel = XWhitePixel(win->_display, 0);
    
   // create the application window
   win->_frame = XCreateWindow(win->_display, 
               RootWindow(win->_display, 0),
               0, 0, 400, 400, 5, 
               depth,
               InputOutput, 
               win->_visual, 
               CWBackPixel,
               &frame_attributes);
    
   XStoreName(win->_display, win->_frame, title.c_str());
    
   XSelectInput(win->_display, win->_frame, ExposureMask | StructureNotifyMask);
    
   XMapWindow(win->_display, win->_frame);

   return win;
}

int32_t X11Window::pump()
{
   if (!_display){
      return 0;
   }

   XEvent evnt;
   XFontStruct* fontinfo;
    
   int text_x;
   int text_y;
    
    
   for (;;) 
   {
      XNextEvent(_display, (XEvent*)&evnt);
      
      switch (evnt.type ) 
      {
         case Expose:
            {
               /*XWindowAttributes window_attributes;
               int font_direction, font_ascent, font_descent;
               
               XCharStruct text_structure;
               XTextExtents(fontinfo,
                  hello_string, hello_string_length, 
               &font_direction, &font_ascent, &font_descent, 
               &text_structure);
               XGetWindowAttributes(display, frame_window, &window_attributes);
               text_x = (window_attributes.width - text_structure.width)/2;
               text_y = (window_attributes.height - 
               (text_structure.ascent+text_structure.descent))/2;
               XDrawString(display, frame_window, graphical_context,
               text_x, text_y, hello_string, hello_string_length);*/
            
            }
            break;
         
         default: break;
      }
   }

   return 0;
}

void X11Window::show() const
{
}

void X11Window::update() const
{
}

void X11Window::close()
{
}

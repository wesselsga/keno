#include "server.h"
#include "channel.h"
#include "window_linux.h"

using namespace win;

Window::Window(const _ctx&)
{
}

Window::~Window()
{
	
}

std::shared_ptr<Window> Window::create(
         const std::string& title,
			const std::weak_ptr<Channel>& channel)
{
	std::shared_ptr<Window> win(std::make_shared<Window>(_ctx{}));
	win->_channel = channel;

	LOG(VERBOSE) << "X11: creating native window...";

   int                     depth;
    XSetWindowAttributes    frame_attributes;
    Window                  frame_window;
    XFontStruct             *fontinfo;
    XGCValues               gr_values;
    GC                      graphical_context;
    XKeyEvent               event;
    char                    hello_string[] = "Hello World";
    int                     hello_string_length = strlen(hello_string);

    display = XOpenDisplay(NULL);
    visual = DefaultVisual(display, 0);
    depth  = DefaultDepth(display, 0);
    
    frame_attributes.background_pixel = XWhitePixel(display, 0);
    
    // create the application window
    frame_window = XCreateWindow(display, XRootWindow(display, 0),
                                 0, 0, 400, 400, 5, depth,
                                 InputOutput, visual, CWBackPixel,
                                 &frame_attributes);
    
    XStoreName(display, frame_window, "Hello World Example");
    
    XSelectInput(display, frame_window, ExposureMask | StructureNotifyMask);

    //fontinfo = XLoadQueryFont(display, "10x20");
    //gr_values.font = fontinfo->fid;
    //gr_values.foreground = XBlackPixel(display, 0);
    //graphical_context = XCreateGC(display, frame_window, 
    //                              GCFont+GCForeground, &gr_values);
    
    XMapWindow(display, frame_window);

    
    return win;
}

int32_t Window::pump()
{
   XEvent evnt;
   XFontStruct* fontinfo;
    
   int text_x;
   int text_y;
    
    
   for (;;) 
   {
      XNextEvent(display, (XEvent*)&evnt);
      
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



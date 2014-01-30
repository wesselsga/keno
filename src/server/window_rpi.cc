#include "server.h"

#include <bcm_host.h>
#include <EGL/egl.h>

#include "window_rpi.h"

using namespace rpi;

Window::Window() : _eglw(0)
{
}

Window::~Window()
{
	delete ((EGL_DISPMANX_WINDOW_T*)_eglw);
}

std::shared_ptr<Window> Window::create()
{
	LOG(VERBOSE) << "rpi: creating native window...";

	bcm_host_init();
		
	uint32_t display_width;
   uint32_t display_height;

   // create an EGL window surface, passing context width/height
   int32_t success = graphics_get_display_size(0 /* LCD */, &display_width, &display_height);
   if ( success < 0 )
   {
		LOG(ERR) << "rpi: failed to query display size.";
      return nullptr;
   }

	LOG(VERBOSE) << "rpi: display size " << display_width << "x" << display_height;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

	std::shared_ptr<Window>win(new Window());
   
   // You can hardcode the resolution here:
   //display_width = 640;
   //display_height = 480;

   dst_rect.x = display_width - 640;
   dst_rect.y = display_height - 480;
   dst_rect.width = 640;
   dst_rect.height = 480;
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = display_width << 16;
   src_rect.height = display_height << 16;   

   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );
         
   dispman_element = vc_dispmanx_element_add(
				dispman_update, 
				dispman_display,
				0/*layer*/, 
				&dst_rect, 
				0/*src*/,
				&src_rect, 
				DISPMANX_PROTECTION_NONE, 
				0 /*alpha*/, 
				0/*clamp*/, 
				DISPMANX_NO_ROTATE);

   if (dispman_element)
   {
      LOG(ERR) << "rpi: failed to add display element.";
   }

	EGL_DISPMANX_WINDOW_T* eglw = new EGL_DISPMANX_WINDOW_T;
	eglw->element = dispman_element;
	eglw->width = display_width;
   eglw->height = display_height;
   
	win->_eglw = (void*)eglw;
	
	vc_dispmanx_update_submit_sync( dispman_update );
   
   return win;
}

void Window::wait()
{
}



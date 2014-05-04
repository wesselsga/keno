#include "server.h"

#include "window_raspi.h"

#include <bcm_host.h>
#include <EGL/egl.h>


using namespace screen;

BcmWindow::BcmWindow(const _ctx&, 
      const std::weak_ptr<Channel>& channel) 
      : Window(channel), _eglw(nullptr) 
{
}

BcmWindow::~BcmWindow()
{
	delete ((EGL_DISPMANX_WINDOW_T*)_eglw);
}

std::shared_ptr<BcmWindow> BcmWindow::create(
            const std::string&,
            const std::weak_ptr<Channel>& channel)
{
	LOG(VERBOSE) << "raspi: creating native window...";

	bcm_host_init();
		
	uint32_t screenWidth;
   uint32_t screenHeight;

   // create an EGL window surface, passing context width/height
   int32_t success = graphics_get_display_size(0 /* LCD */, &screenWidth, &screenHeight);
   if ( success < 0 )
   {
		LOG(ERR) << "raspi: failed to query display size.";
      return nullptr;
   }

	LOG(VERBOSE) << "raspi: display size " << screenWidth << "x" << screenHeight;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

	std::shared_ptr<BcmWindow>win(new BcmWindow(_ctx{}, channel));
   
   dst_rect.width = 1024;
   dst_rect.height = 512;
   dst_rect.x = screenWidth - dst_rect.width;
   dst_rect.y = screenHeight - dst_rect.height;   
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = screenWidth << 16;
   src_rect.height = screenHeight << 16;   

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

   if (!dispman_element)
   {
      LOG(ERR) << "raspi: failed to add display element.";
   }

	EGL_DISPMANX_WINDOW_T* eglw = new EGL_DISPMANX_WINDOW_T;
	eglw->element = dispman_element;
	eglw->width = screenWidth;
   eglw->height = screenHeight;
   
	win->_eglw = (void*)eglw;
	
	vc_dispmanx_update_submit_sync( dispman_update );
   
   return win;
}

int32_t BcmWindow::pump()
{
   return 0;
}

void BcmWindow::show() const
{
}

void BcmWindow::update() const
{
}

void BcmWindow::close()
{
}
	



#include "server.h"
#include "window.h"


#ifdef _WIN32
#include "window_win.h"
#else
#ifdef _RASPI
#include "window_raspi.h"
#else
#include "window_linux.h"
#endif
#endif


screen::Window::Window(const std::weak_ptr<Channel>& channel)
   : _channel(channel)
{
}

screen::Window::~Window()
{
}

std::shared_ptr<screen::Window> screen::Window::create(
         const std::string& title,
			const std::weak_ptr<Channel>& channel)
{
   std::shared_ptr<screen::Window> win;
	
#ifdef _WIN32
   win = Win32Window::create(title, channel);
#else
#ifdef _RASPI
   win = BcmWindow::create(title, channel);
#else
   win = X11Window::create(title, channel);
#endif
#endif
	
   return win;
}
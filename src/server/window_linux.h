#pragma once

class Channel;

#include <X11/Xlib.h>

namespace x11 {

class Window
{
	struct _ctx { };
public:
	explicit Window(const _ctx&);
	virtual ~Window();

	static std::shared_ptr<Window> create(
            const std::string&,
            const std::weak_ptr<Channel>&);
   
	int32_t pump();

   void* display() const { return _display; }

private:

	Window()=delete;
	Window(const Window&)=delete;
	const Window& operator=(const Window&) = delete;

	std::weak_ptr<Channel> _channel;
	
   Display* _display;
   Visual*  _visual;
   ::Window*  _frame;


};

};

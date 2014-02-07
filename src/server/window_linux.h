#pragma once

class Channel;

#include <X11/Xlib.h>

namespace win {

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

private:

	Window()=delete;
	Window(const Window&)=delete;
	const Window& operator=(const Window&) = delete;

	static LRESULT CALLBACK _winProc(HWND,UINT,WPARAM,LPARAM);
	bool winProc(uint32_t, WPARAM, LPARAM, LRESULT&);

	bool fullScreen(uint32_t, uint32_t);
	
	std::weak_ptr<Channel> _channel;
	
   Display* _display;
   Visual*  _visual;


};

};

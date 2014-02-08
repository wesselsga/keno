#pragma once

#include "window.h"
#include <X11/Xlib.h>

namespace screen {

class X11Window : public Window
{
	struct _ctx { };
public:
	explicit X11Window(const _ctx&, const std::weak_ptr<Channel>&);
	virtual ~X11Window();

	static std::shared_ptr<Window> create(
            const std::string&,
            const std::weak_ptr<Channel>&);
   
   void*   handle() const { return _frame; }
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
   ::Window*  _frame;

};

};

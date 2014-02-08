#pragma once

#include "window.h"

namespace screen {

class Win32Window : public Window
{
	struct _ctx { };
public:
	explicit Win32Window(const _ctx&,const std::weak_ptr<Channel>&);
	virtual ~Win32Window();

	static std::shared_ptr<Win32Window> create(
            const std::string&,
            const std::weak_ptr<Channel>&);

	void* handle() const { return _hwnd; }
   void* display() const { return nullptr; }
	void  show() const;
	void  update() const;
	void  close();

	int32_t pump();

private:

	Win32Window()=delete;
	Win32Window(const Win32Window&)=delete;
	const Win32Window& operator=(const Win32Window&) = delete;

	static LRESULT CALLBACK _winProc(HWND,UINT,WPARAM,LPARAM);
	bool winProc(uint32_t, WPARAM, LPARAM, LRESULT&);

	bool fullScreen(uint32_t, uint32_t);
	
	HWND   _hwnd;
	bool _fullscreen;

};

};

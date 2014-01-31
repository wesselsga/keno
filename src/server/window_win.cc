#include "server.h"
#include "channel.h"
#include "window_win.h"

using namespace win;

Window::Window(const _ctx&) : _hwnd(0)
{
	_fullscreen = false;
}

Window::~Window()
{
	if (::IsWindow(_hwnd)) {
		::CloseWindow(_hwnd);
	}
}

std::shared_ptr<Window> Window::create(
         const std::string& title,
			const std::weak_ptr<Channel>& channel)
{
	std::shared_ptr<Window> win(std::make_shared<Window>(_ctx{}));
	win->_channel = channel;

	LOG(VERBOSE) << "win32: creating native window...";

	const wchar_t* clsname = L"_mainw";

	HINSTANCE module = ::GetModuleHandle(0);

	{ // win class registration

		WNDCLASSEX wcls;
		wcls.cbSize = sizeof(WNDCLASSEX);
		wcls.cbClsExtra = 0;
		wcls.cbWndExtra = 0;
		//wcls.hbrBackground = 0;
		wcls.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wcls.hCursor = ::LoadCursor(0, IDC_ARROW);
		wcls.hIcon = 0;
		wcls.hIconSm = 0;
		wcls.hInstance = module;
		wcls.lpfnWndProc = _winProc;
		wcls.lpszClassName = clsname;
		wcls.lpszMenuName = 0;
		wcls.style = CS_OWNDC;
		::RegisterClassEx(&wcls);
	}

	uint32_t w = 640;
	uint32_t h = 480;

	DWORD style = WS_OVERLAPPEDWINDOW;
	DWORD exstyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

	/*if (win->fullScreen(w, h))
	{
		style = WS_POPUP;
		exstyle = WS_EX_APPWINDOW;		
	}*/
	
	// rc is the desired client size we want
	RECT rc = {0, 0, w, h};
	::AdjustWindowRectEx(&rc, style, FALSE, 0);
	
	HWND hwnd = ::CreateWindowEx(exstyle,
					clsname,
					convert::to_utf16(title).c_str(),
					style,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					rc.right - rc.left,
					rc.bottom - rc.top,
					0,
					0,
					module,
					(LPVOID)win.get());

	if (!::IsWindow(hwnd)) {
		return nullptr;
	}

	return win;
}

int32_t Window::pump()
{
	int32_t ret=0;

	MSG msg;
	while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			ret = -1;
			break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
		++ret;
	}	

	return ret;
}

void Window::show() const
{
	::ShowWindow(handle(), SW_SHOWNORMAL);
}

void Window::update() const
{
	::UpdateWindow(handle());
}

LRESULT CALLBACK Window::_winProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp)
{
	Window* pwin;
	if (msg == WM_NCCREATE)
	{
		auto cs = reinterpret_cast<CREATESTRUCT*>(lp);		
		pwin = reinterpret_cast<Window*>(cs->lpCreateParams);
		if (pwin)
		{
			pwin->_hwnd = hwnd;
			::SetWindowLongPtr(hwnd,GWLP_USERDATA,(DWORD_PTR)pwin);
		}		
	}
	else
	{
		pwin = (Window*)::GetWindowLongPtr(hwnd,GWLP_USERDATA);		
	}

	if (pwin)
	{
		LRESULT lr;
		if (pwin->winProc(msg, wp, lp, lr)) {
			return lr;
		}
	}

	return ::DefWindowProc(hwnd, msg ,wp, lp);		
}

bool Window::winProc(uint32_t msg, WPARAM wp, LPARAM, LRESULT& result)
{
	result = 0;

	switch (msg)
	{
		case WM_KEYDOWN:
			if (wp == VK_ESCAPE){
				::PostMessage(_hwnd, WM_CLOSE, 0, 0);
			}
			break;

		case WM_SIZE:
			break;

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				::BeginPaint(_hwnd, &ps);

				::EndPaint(_hwnd, &ps);
			}
			return true;
		
		case WM_DESTROY:

			if (_fullscreen)
			{
				ChangeDisplaySettings(NULL,0);
				ShowCursor(TRUE);
			}

			::PostMessage(_hwnd, WM_QUIT, 1, 0);
			break;

		case WM_NCDESTROY:
			_hwnd = 0;
			break;

		default: break;
	}

	return false;
}

bool Window::fullScreen(uint32_t width, uint32_t height)
{
	DEVMODE mode;
	memset(&mode, 0, sizeof(mode));
	mode.dmSize = sizeof(mode);
	mode.dmPelsWidth = width;
	mode.dmPelsHeight = height;
	mode.dmBitsPerPel = 32;
	mode.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	LONG ret = ::ChangeDisplaySettings(&mode, CDS_FULLSCREEN);
	if (ret != DISP_CHANGE_SUCCESSFUL) 
	{
		LOG(ERR) << "win: failed to change display settings! ("
			<< width << "x" << height << ")";
		_fullscreen = false;
	}
	else
	{
		LOG(ERR) << "win: changed to fullscreen: ("
			<< width << "x" << height << ")";
		
		_fullscreen = true;
	}
	
	return _fullscreen;
}

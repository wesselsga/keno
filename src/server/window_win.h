#pragma once

class Channel;

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

	HWND handle() const { return _hwnd; }
	void show() const;
	void update() const;
	void close();

	int32_t pump();

private:

	Window()=delete;
	Window(const Window&)=delete;
	const Window& operator=(const Window&) = delete;

	static LRESULT CALLBACK _winProc(HWND,UINT,WPARAM,LPARAM);
	bool winProc(uint32_t, WPARAM, LPARAM, LRESULT&);

	bool fullScreen(uint32_t, uint32_t);
	
	HWND   _hwnd;
	std::weak_ptr<Channel> _channel;
	bool _fullscreen;

};

};

#ifndef __WINDOW_RPI_H__
#define __WINDOW_RPI_H__

namespace rpi {

class Window
{
public:
	virtual ~Window();

	static std::shared_ptr<Window> create();

	void* handle() const { return _eglw; }
	//void show() const;
	//void update() const;

	void wait();

private:

	Window();

	void* _eglw;		

};

};

#endif // __WINDOW_RPI_H__
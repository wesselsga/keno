#ifndef __CONTEXT_WIN_H__
#define __CONTEXT_WIN_H__


namespace gfx {

class WglContext : public Context
{
public:
	~WglContext();

	static std::shared_ptr<Context> create(void*);
	
	void bind();
	void unbind();
	void clear();
	void swapBuffers();
	void finish();

private:
	WglContext();

	HGLRC _hrc;
	HDC   _hdc;
};

}; // gfx


#endif //  __CONTEXT_WIN_H__
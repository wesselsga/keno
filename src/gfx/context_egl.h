#pragma once

namespace gfx {
	
class EglContext : public Context
{
public:
	~EglContext();

	static std::shared_ptr<Context> create(void*);
	
	void bind();
	void unbind();
	void clear();
	void swapBuffers();
	void finish();

private:
	EglContext();

	void* _ctx;
	void* _display;
	void* _surface;

};

}; // gfx
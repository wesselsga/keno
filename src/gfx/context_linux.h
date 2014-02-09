#pragma once

#include <GL/glew.h>
#include <GL/glxew.h>

namespace gfx {

class GlxContext : public Context
{
public:
	~GlxContext();

	static std::shared_ptr<Context> create(void*,void*);
	
	void bind();
	void unbind();
	void clear();
	void swapBuffers();
	void finish();

private:
	GlxContext();

   ::GLXContext _ctx;

   Display* _display;
   GLXDrawable _window;


};

}; // gfx

#pragma once

namespace gfx {

class GlxContext : public Context
{
public:
	~GlxContext();

	static std::shared_ptr<Context> create(void*);
	
	void bind();
	void unbind();
	void clear();
	void swapBuffers();
	void finish();

private:
	GlxContext();

   ::GLXContext _ctx;

};

}; // gfx
#pragma once

namespace gfx {

class Context
{
public:
	virtual ~Context(){}

	static std::shared_ptr<Context> create(void*);

	virtual void bind()=0;
	virtual void unbind()=0;

	virtual void clear()=0;

	virtual void swapBuffers()=0;

	virtual void finish()=0;


};

};
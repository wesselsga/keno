#pragma once

namespace trace {

//
// write trace messages to a uv_pipe_t
//
class PipeWriter : public trace::Writer
{
public:
	PipeWriter(uv_pipe_t*);
	~PipeWriter();

	void writeLine(int32_t,const std::string&);

private:
	uv_pipe_t* _pipe;

   static void after_write(uv_write_t*, int);
};

};

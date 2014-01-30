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

	size_t _bufcount;
	size_t _bufsize;
	char** _buffers;
	size_t _bufindex;

	static void after_write(uv_write_t*, int);
};

};

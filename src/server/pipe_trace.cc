#include "server.h"
#include "pipe_trace.h"

using namespace trace;

PipeWriter::PipeWriter(uv_pipe_t* pipe) 
	: _pipe(pipe)
{
	_bufindex = 0;
	_bufcount = 16;
	_bufsize = 1024;

	_buffers = new char*[_bufcount];
	for (size_t n = 0; n < _bufcount; n++) {
		_buffers[n] = new char[_bufsize];
	}
}

PipeWriter::~PipeWriter()
{
	for (size_t n = 0; n < _bufcount; n++){
		delete [] _buffers[n];
	}	
	delete [] _buffers;
}

void PipeWriter::after_write(uv_write_t* req, int status) 
{
	//ASSERT(status == 0);	
	delete req;
}

void PipeWriter::writeLine(int32_t,const std::string& msg)
{
	uv_write_t* req = new uv_write_t;
	
	strncpy(_buffers[_bufindex], msg.c_str(), _bufsize-1);	
	uv_buf_t buf = uv_buf_init(_buffers[_bufindex], msg.size());	
	
	if (uv_write(req, 
				(uv_stream_t*)_pipe, 
				&buf, 1, 
				after_write))
	{
		delete req;
	}

	_bufindex = ((_bufindex + 1) % _bufcount);
}

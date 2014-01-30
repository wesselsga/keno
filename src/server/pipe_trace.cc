#include "server.h"
#include "pipe_trace.h"

using namespace trace;

PipeWriter::PipeWriter(uv_pipe_t* pipe) 
	: _pipe(pipe)
{	
}

PipeWriter::~PipeWriter()
{
}

void PipeWriter::after_write(uv_write_t* req, int status) 
{
   // clean-up both the request and the buffer
   if (req)
   {
      if (req->data){
         free(req->data);
      }
      free(req);
   }
}

void PipeWriter::writeLine(int32_t,const std::string& msg)
{
   size_t len = msg.size();
   if (!len){
      return;
   }

   // allocate a write request
	uv_write_t* req = reinterpret_cast<uv_write_t*>(
         malloc(sizeof(uv_write_t)));
   if (!req){
      return;
   }
   
   memset(req, 0x00, sizeof(uv_write_t));
   
   // allocate a buffer for the text
   req->data = malloc(len);
   if (req->data)
   {
      memcpy(req->data, msg.c_str(), len);         
      uv_buf_t buf = uv_buf_init(reinterpret_cast<char*>(req->data), len);		
	   if (!uv_write(req, 
               (uv_stream_t*)_pipe, 
				   &buf, 1, after_write))
	   {
		   return;
	   }
      free(req->data);
   }

   free(req);
}

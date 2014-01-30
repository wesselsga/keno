#include "server.h"
#include "channel_host.h"


static void on_alloc(uv_handle_t* handle,
                     size_t suggested,
                     uv_buf_t* buf) 
{
   
   //LOG(VERBOSE) << "libuv: on_alloc " << suggested;

   buf->len = suggested + 64;
   buf->base = (char*)malloc(buf->len);
}

static void on_close(uv_handle_t* handle) 
{
   free(handle);
}

static void on_read(uv_stream_t* pipe, ssize_t n, const uv_buf_t* rdbuf) 
{
   if (!rdbuf->base){
      return;
   }

   if (n <= 0)
   {
      free(rdbuf->base);
      return;
   }
   
   std::string str(rdbuf->base, n);
   trace::writeLine(0, str);

   free(rdbuf->base);
}


ChannelHost::ChannelHost(const _priv&, 
      const std::weak_ptr<Server>& server, 
      const std::string& id) 
   : _server(server), _id(id)
{
   _process = nullptr;
   _out = nullptr;
   memset(&_options, 0x00, sizeof(_options));
}
   
ChannelHost::~ChannelHost()
{
}

std::shared_ptr<ChannelHost> ChannelHost::create(
         uv_loop_t* loop,
         const std::weak_ptr<Server>& server, 
         const std::string& id)
{
   std::shared_ptr<ChannelHost> host(
         std::make_shared<ChannelHost>(_priv{}, server, id));
      
   char path[1024];
   size_t pathSize = 1024;
   if (uv_exepath(path, &pathSize)){
      return nullptr;
   }
   //path[pathSize] = '\0';

   std::string str("--channel=");
   str.append(id);

   char* args[3];
   args[0] = path;
   args[1] = const_cast<char*>(str.c_str());
   args[2] = NULL;
      
   host->_options.flags = 0;
   host->_options.file = path;
   host->_options.args = args;
   host->_options.exit_cb = onExit;		

   // alloc and initialize a pipe
   host->_out = reinterpret_cast<uv_pipe_t*>(malloc(sizeof(uv_pipe_t)));
   memset(host->_out, 0x00, sizeof(uv_pipe_t));
   uv_pipe_init(loop, host->_out, 0);

   uv_stdio_container_t stdio[3];
   stdio[0].flags = UV_IGNORE;
   stdio[0].data.stream = nullptr;
   stdio[1].flags = (uv_stdio_flags)(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
   stdio[1].data.stream = (uv_stream_t*)host->_out;
   //stdio[1].flags = UV_IGNORE;
   //stdio[1].data.stream = nullptr;
   stdio[2].flags = UV_IGNORE;
   stdio[2].data.stream = nullptr;
      
   host->_options.stdio = stdio;
   host->_options.stdio_count = 3;

   LOG(VERBOSE) << "spawn: " << path << " " << str;

   // alloc and spawn a process
   host->_process = reinterpret_cast<uv_process_t*>(malloc(sizeof(uv_process_t)));
   memset(host->_process, 0x00, sizeof(uv_process_t));
      
   if (uv_spawn(loop, host->_process, &host->_options)) 
   {
      uv_close((uv_handle_t*)host->_process, on_close);
      uv_close((uv_handle_t*)host->_out, on_close);
      return nullptr;
   }	
   
   // attach our host to the process as data
   host->_process->data = host.get();

   uv_read_start((uv_stream_t*)host->_out, on_alloc, on_read);
   
   return host;
}

void ChannelHost::onExit(uv_process_t* process, 
         int64_t exit_status, int term_signal) 
{
   // first, close the process handle
   uv_close((uv_handle_t*)process, on_close);

   LOG(VERBOSE) << "host: process exit:" 
         << exit_status << ", signal:" << term_signal;

   // next, close the pipe handle as well
   ChannelHost* host = reinterpret_cast<ChannelHost*>(process->data);	 
   if (host)
   {
      uv_close((uv_handle_t*)host->_out, on_close);
   
      // detach ourself from the server
      auto svr = host->_server.lock();
      if (svr){
         svr->removeHost(host);
      }
   }	
}

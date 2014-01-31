#include "server.h"
#include "channel_host.h"
#include "../version.h"

static void alloc_cb(uv_handle_t* handle,
                     size_t suggested_size,
                     uv_buf_t* buf) 
{
   buf->len = suggested_size + 128;
   buf->base = reinterpret_cast<char*>(malloc(buf->len));
}

static void send_cb(uv_udp_send_t* req, int status)
{

}

static void recv_packet(uv_udp_t* handle,
                       ssize_t nread,
                       const uv_buf_t* buf,
                       const struct sockaddr* addr,
                       unsigned flags) 
{
   if (!buf->base){
      return;
   }

   LOG(VERBOSE) << "net: pkt" << std::string(buf->base, nread);

   free(buf->base);

  //CHECK_HANDLE(handle);
  //ASSERT(flags == 0);

 // cl_recv_cb_called++;

  //if (nread < 0) {
    //ASSERT(0 && "unexpected error");
  //}

  //if (nread == 0) {
    /* Returning unused buffer */
    /* Don't count towards cl_recv_cb_called */
    //ASSERT(addr == NULL);
    //return;
  //}

  //ASSERT(addr != NULL);
  //ASSERT(nread == 4);
  //ASSERT(!memcmp("PING", buf->base, nread));

  /* we are done with the client handle, we can close it */
  //uv_close((uv_handle_t*) &client, close_cb);
}


Server::Server(const _priv&)
{
}

Server::~Server()
{
}

int32_t Server::run()
{
   LOG(INFO) << "server: starting " 
			<< PRODUCT << " (v" << VERSION_STR << ") ...";
	
	std::shared_ptr<Server> server = std::make_shared<Server>(_priv{});

	server->_self = server;

	auto* loop = uv_default_loop();

	const uint32_t channels = 1;
	for (uint32_t n = 0; n < channels; ++n)
	{
		auto host = ChannelHost::create(
					loop, 
					server, 
					convert::to_utf8(n));
		server->_hosts.push_back(host);	
	}

	int ret;
	
	const char* maddr = "239.255.0.1";
	int32_t port = 2115;

	uv_udp_t listener;
	uv_udp_init(loop, &listener);
	struct sockaddr_in addr;
	uv_ip4_addr("0.0.0.0", port, &addr);	
	uv_udp_bind(&listener, (const struct sockaddr*)&addr, 0);
	
	ret = uv_udp_set_membership(&listener, maddr, NULL, UV_JOIN_GROUP);

	ret = uv_udp_recv_start(&listener, alloc_cb, recv_packet);

	LOG(INFO) << "net: listening on udp://" << maddr << ":" << port << " ...";
   
   uv_udp_send_t* send_req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
   
   uv_buf_t buf = uv_buf_init("HELLO", 5);
   
   uv_udp_send(send_req,
            &listener, 
            &buf, 
            1,
            (const struct sockaddr*)&addr, 
            send_cb);
    
	uv_run(loop, UV_RUN_DEFAULT);

	LOG(INFO) << "server: exit.";	

	return 0;
}

bool Server::removeHost(ChannelHost* host)
{
	std::lock_guard<std::mutex> guard(_lock);
	for (auto i = _hosts.begin(); i != _hosts.end(); i++)
	{
		if ((*i).get() == host)
		{
			LOG(VERBOSE) << "server: detaching channel: " << host->id();
			_hosts.erase(i);

			if (_hosts.empty()){
				//close();
			}

			return true;
		}
	}
	return false;
}

void Server::close()
{
	uv_stop(uv_default_loop());
}

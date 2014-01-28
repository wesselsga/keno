#include "client.h"
#include "../version.h"

static void alloc_cb(uv_handle_t* handle,
                     size_t suggested_size,
                     uv_buf_t* buf) {
  static char slab[65536];
  //CHECK_HANDLE(handle);
  //ASSERT(suggested_size <= sizeof(slab));
  buf->base = slab;
  buf->len = sizeof(slab);
}

static void recv_packet(uv_udp_t* handle,
                       ssize_t nread,
                       const uv_buf_t* buf,
                       const struct sockaddr* addr,
                       unsigned flags) {
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


int32_t main(int32_t argc, char* argv[])
{
	int32_t ret = 0;	

	{
		auto* loop = uv_default_loop();

		char* maddr = "239.255.0.1";
		int32_t port = 2115;

		uv_udp_t listener;
		uv_udp_init(loop, &listener);
		struct sockaddr_in recv_addr;
		uv_ip4_addr("0.0.0.0", port, &recv_addr);	
		uv_udp_bind(&listener, (const struct sockaddr*)&recv_addr, 0);
	
		ret = uv_udp_set_membership(&listener, maddr, NULL, UV_JOIN_GROUP);

		ret = uv_udp_recv_start(&listener, alloc_cb, recv_packet);

		LOG(INFO) << "net: listening on udp://" << maddr << ":" << port << " ...";

		uv_run(loop, UV_RUN_DEFAULT);
	
	}

	LOG(INFO) << "client: exit.";
	return ret;
}
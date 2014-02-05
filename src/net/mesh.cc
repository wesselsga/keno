#include "net.h"

using namespace mesh;

static void alloc_cb(uv_handle_t* handle,
                     size_t suggested_size,
                     uv_buf_t* buf) 
{
   buf->len = suggested_size + 128;
   buf->base = reinterpret_cast<char*>(malloc(buf->len));
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

   if (nread)
   {
      auto frame = Frame::parse(buf->base, nread);

      LOG(VERBOSE) << "mesh: pkt - " << std::string(buf->base, nread);
   }

   free(buf->base);
}

static void send_cb(uv_udp_send_t* req, int)
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

const char* _mcastAddr = "224.1.1.1";
int32_t     _port = 2115;

Node::Node(const Uuid& id) : _id(id)
{
   memset(&_transport, 0x00, sizeof(_transport));
}

Node::~Node()
{
   close();
}

bool Node::open(uv_loop_t* loop)
{
   if (!loop){
      return false;
   }

	uv_udp_init(loop, &_transport);
   
	struct sockaddr_in addr;
	uv_ip4_addr("0.0.0.0", _port, &addr);	
	
   int ret = uv_udp_bind(&_transport, (const struct sockaddr*)&addr, 0);
   if (ret)
   {
      LOG(ERR) << "mesh: error binding udp listener";
      return false;
   }
	
	ret = uv_udp_set_membership(&_transport, _mcastAddr, NULL, UV_JOIN_GROUP);
   if (ret)
   {
      LOG(ERR) << "mesh: error joining multicast group";
      return false;
   }

   uv_udp_set_multicast_loop(&_transport, 0);

	ret = uv_udp_recv_start(&_transport, alloc_cb, recv_packet);
   if (ret)
   {
      LOG(ERR) << "mesh: error reading udp";
      return false;
   }

	LOG(INFO) << "mesh: " << 
         id() << " open @ udp://" << _mcastAddr << ":" << _port << " ...";
      
   send(nullptr);

   return true;
}

void Node::close()
{
   
}

bool Node::send(Frame*)
{
   uv_udp_send_t* send_req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
   
   char* data = reinterpret_cast<char*>(malloc(28));
   data[0] = 0x1C;
   data[1] = 0x90;
   data[2] = 0x00;
   data[3] = 0x00;
   memset(&data[4], 0x00, 16);
   data[20] = 0x00;
   data[21] = 0x00;
   data[22] = 0x00;
   data[23] = 0x00;
   data[24] = 0x00;
   data[25] = 0x00;
   data[26] = 0x00;
   data[27] = 0x00;

   send_req->data = data;

   uv_buf_t buf = uv_buf_init(data, 28);
   
   struct sockaddr_in addr;
	uv_ip4_addr(_mcastAddr, _port, &addr);	
	
   uv_udp_send(send_req,
            &_transport, 
            &buf, 
            1,
            (const struct sockaddr*)&addr, 
            send_cb);

   return true;
}


Frame::~Frame()
{
}

std::shared_ptr<Frame> Frame::parse(char* buf, size_t cb)
{
   if (!buf || !cb){
      return nullptr;
   }

   if (cb < 28){
      return nullptr;
   }

     


   return nullptr;
}
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

      LOG(VERBOSE) << "mesh: frame (size" << nread << ")";
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

   uv_udp_set_multicast_loop(&_transport, 1);

	ret = uv_udp_recv_start(&_transport, alloc_cb, recv_packet);
   if (ret)
   {
      LOG(ERR) << "mesh: error reading udp";
      return false;
   }

	LOG(INFO) << "mesh: " << 
         id() << std::endl << "  open @ udp://" << _mcastAddr << ":" << _port;
      
   send(nullptr);

   return true;
}

void Node::close()
{
   
}

bool Node::send(Frame*)
{
   uv_udp_send_t* send_req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
   
   uint16_t len = 37;
   len = htons(len);

   char* data = reinterpret_cast<char*>(malloc(28));
   data[0] = 0x1C;
   data[1] = 0x90;
   memcpy(data + 2, &len, 2);

   uint32_t d0 = htonl(_id.data0());
   uint32_t d1 = htonl(_id.data1());
   uint32_t d2 = htonl(_id.data2());
   uint32_t d3 = htonl(_id.data3());

   if (htonl(47) != 47) 
   {
      uint32_t tmp = d0;
      d0 = d3;
      d3 = tmp;
      tmp = d1;
      d1 = d2;
      d2 = tmp;
   }
   memcpy(data + 4, &d0, 4);
   memcpy(data + 8, &d1, 4);
   memcpy(data + 12, &d2, 4);
   memcpy(data + 16, &d3, 4);

   uint32_t msgid = 0x00AA00FF;
   msgid = htonl(msgid);
   memcpy(data + 20, &msgid, 4);
   
   uint32_t seqno = 0x00BB00EE;
   seqno = htonl(seqno);
   memcpy(data + 24, &seqno, 4);

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


Frame::Frame(const Uuid& node) : _node(node)
{
}

Frame::~Frame()
{
}

std::shared_ptr<Frame> Frame::parse(const char* buf, size_t cb)
{
   if (!buf || !cb){
      return nullptr;
   }

   if (cb < 28){
      return nullptr;
   }

   const char* p = buf;

   // copy version and control flags
   uint8_t version = ((*p >> 4) & 0x0F);
   uint8_t flags = ((*p << 4) & 0xF0) | ((*(p+1) >> 4) & 0x0F);  

   // we only support version 0x01
   if (version != 0x01){
      return nullptr;
   }
   
   // copy payload length
   uint16_t len;
   memcpy(&len, p + 2, 2);
   len = ntohs(len);
   
   uint32_t d1,d2,d3,d4;
   memcpy(&d1, p + 4, 4);
   memcpy(&d2, p + 8, 4);
   memcpy(&d3, p + 12, 4);
   memcpy(&d4, p + 16, 4);
      
   d1 = ntohl(d1);
   d2 = ntohl(d2);
   d3 = ntohl(d3);
   d4 = ntohl(d4);

   // are we little-endian?
   if (htonl(47) != 47) 
   {
      uint32_t tmp = d1;
      d1 = d4;
      d4 = tmp;
      tmp = d2;
      d2 = d3;
      d3 = tmp;
   }
         
   // create a frame with node-id
   auto frame = std::make_shared<Frame>(Uuid(d1,d2,d3,d4));

   frame->_version = version;
   frame->_flags = flags;
   frame->_length = len;   
   
   memcpy(&frame->_msgid, p + 20, 4);
   frame->_msgid = ntohl(frame->_msgid);
   
   memcpy(&frame->_seqno, p + 24, 4);   
   frame->_seqno = ntohl(frame->_seqno);
   
   LOG(VERBOSE) << frame->node();

   return frame;
}
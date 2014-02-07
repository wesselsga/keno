#pragma once


namespace mesh {

class Node;
class Frame;
class Message;

class Node
{
public:
   Node(const Uuid&);
   virtual ~Node();

   const Uuid& id() const { return _id; }

   bool open(uv_loop_t*);
   void close();

private:
      
   bool send(Frame*);

   Uuid _id;

   uv_udp_t _transport;

};

class Frame
{
public:
   Frame(const Uuid&);

   virtual ~Frame();
   
   static std::shared_ptr<Frame> parse(const char*, size_t);

   const Uuid& node() const { return _node; }

private:

   uint8_t  _version;
   uint8_t  _flags;
   uint16_t _length;
   Uuid     _node;
   uint32_t _msgid;
   uint32_t _seqno;
   

};


class Message : public Frame
{
public:


};


};
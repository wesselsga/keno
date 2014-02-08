#pragma once

class Channel;

namespace screen {

class Window
{
public:
	virtual ~Window();

	static std::shared_ptr<Window> create(
            const std::string&,
            const std::weak_ptr<Channel>&);

	virtual void* handle() const=0;
   virtual void* display() const=0;
	virtual void  show() const=0;
	virtual void  update() const=0;
	virtual void  close()=0;

   virtual int32_t pump()=0;

protected:

   Window(const std::weak_ptr<Channel>&);

   const std::weak_ptr<Channel> _channel;
	

};

};

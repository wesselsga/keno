#pragma once

#include <core.h>
#include <gfx.h>

#include <string>
#include <list>

#include <thread>
#include <mutex>
#include <condition_variable>

class Channel;

class Server
{
	struct _priv { };
public:
	explicit Server(const _priv&);
	virtual ~Server();
	
	static int32_t run();

	std::shared_ptr<Channel> openChannel(const std::string&);

protected:

private:
	Server()=delete;
	Server(const Server&)=delete;
	const Server& operator=(const Server&) = delete;

	void close();

	friend class ChannelHost;
	bool removeHost(ChannelHost*);

	std::list< std::shared_ptr<ChannelHost> > _hosts;
	
	std::mutex _lock;
	std::weak_ptr<Server> _self;
};
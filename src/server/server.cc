#include "server.h"
#include "channel_host.h"
#include "config.h"

Server::Server(const _priv&)
{
}

Server::~Server()
{
}

int32_t Server::run()
{
   LOG(INFO) << "server: " << PRODUCT << " (v" << VERSION_STR << ") ...";

  	auto server = std::make_shared<Server>(_priv{});

	server->_self = server;

   // read the configuration so we know 
   // how many channels to spawn
   auto conf = Config::open(); 
   if (!conf)
   {
      LOG(ERR) << "invalid configuration";
      return -1;
   }

   auto loop = uv_default_loop();
   
	for (auto c : conf->channels())
	{
		auto host = ChannelHost::create(
					loop, 
					server, 
					c->name());
		server->_hosts.push_back(host);	
	}
   
   Uuid nid = Uuid::parse("ECBD63F4-2125-48C7-BEBD-0BB95DC982DD");
   //Uuid nid = Uuid::make();
   mesh::Node node(nid);	

   node.open(loop);   
   
    
	uv_run(loop, UV_RUN_DEFAULT);

   node.close();

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

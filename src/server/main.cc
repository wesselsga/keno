#include "server.h"
#include "channel.h"

#include "executor.h"

int32_t main(int32_t argc, char* argv[])
{
	int32_t ret = 0;
	std::string channel;
	
	for (auto arg=0; arg < argc; ++arg)
	{
		if (!strncmp(argv[arg], "--channel=", 10)){
			channel.assign(argv[arg] + 10);
		}
	}

   auto e = Executor::create();

   if (channel.empty()) 
	{
      ret = Server::run();		
	}  
	else
	{

      //auto writer = std::make_shared<trace::FileWriter>("test.log");
		//trace::addWriter(":file", writer);
      
      ret = Channel::run(channel);		
	}

	return ret;
}
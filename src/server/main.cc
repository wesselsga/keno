#include "server.h"
#include "channel.h"

int32_t main(int32_t argc, char* argv[])
{
	int32_t ret = 0;
	std::string channel;
	
	for (int32_t arg=0; arg < argc; ++arg)
	{
		if (!strncmp(argv[arg], "--channel=", 10)){
			channel.assign(argv[arg] + 10);
		}
	}

   {
      // Get the default Isolate created at startup.
      v8::Isolate* isolate = v8::Isolate::GetCurrent();

      // Create a stack-allocated handle scope.
      v8::HandleScope handle_scope(isolate);

      // Create a new context.
      v8::Handle<v8::Context> context = v8::Context::New(isolate);

      // Enter the context for compiling and running the hello world script.
      v8::Context::Scope context_scope(context);

      // Create a string containing the JavaScript source code.
      v8::Handle<v8::String> source = v8::String::NewFromUtf8(isolate, "'Hello' + ', World!'");
  
      // Compile the source code.
      v8::Handle<v8::Script> script = v8::Script::Compile(source);
  
      // Run the script to get the result.
      v8::Handle<v8::Value> result = script->Run();
  
      // Convert the result to an UTF8 string and print it.
      v8::String::Utf8Value utf8(result);
      printf("%s\n", *utf8);
   }

	if (channel.empty()) 
	{
      ret = Server::run();		
	}  
	else
	{

      auto writer = std::make_shared<trace::FileWriter>("test.log");
		trace::addWriter(":file", writer);
      ret = Channel::run(channel);		
	}

	return ret;
}

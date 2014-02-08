#include "server.h"
#include "channel.h"
#include "../version.h"

#include "stopwatch.h"
#include "window.h"

#ifndef _WIN32
#include <unistd.h>
#endif

void prompt()
{
#ifdef _WIN32
	::MessageBox(0, 
			L"Attach debugger?", 
			L"Debugger", 
			MB_YESNO|MB_ICONQUESTION);
#endif
}

void wait_for_a_while(uv_idle_t* handle, int status) 
{
   std::chrono::milliseconds timeout(10);
   std::this_thread::sleep_for(timeout);
}

Channel::Channel(const _priv&, 
		const std::string& id) 
	: _id(id)
{
	_iothread = 0;

	uv_mutex_init(&_lock);
	uv_cond_init(&_isReady);
}
	
Channel::~Channel()
{
	uv_cond_destroy(&_isReady);
	uv_mutex_destroy(&_lock);
}

int32_t Channel::run(const std::string& id)
{
	prompt();

	std::shared_ptr<Channel> channel(
			std::make_shared<Channel>(_priv{}, id));
	
	uv_thread_create(
			&channel->_iothread, 
			iothread, 
			channel.get());

	uv_cond_wait(&channel->_isReady, &channel->_lock);

	LOG(VERBOSE) << "channel: " << id << " opening ...";	
	
	gfx::startup();		

	{
		std::shared_ptr<gfx::Context> ctx;

      std::stringstream title;
      title << PRODUCT << " | " << "channel: " << channel->id();

      auto window = screen::Window::create(title.str(), channel);

#ifdef _WIN32
		
		ctx = gfx::Context::create(window->handle());

#else

#ifdef _RASPI
      
		ctx = gfx::Context::create(window->handle());
#else
 
#endif

#endif
		     
		window->show();
		window->update();

		LOG(DEBUG) << "channel: staring rendering loop ...";

		uint32_t frame = 0;

		int64_t freq = hires_frequency();
		int64_t now, mark = hires_time();

		for (;;)
		{

         if (window->pump() < 0)
			{
				channel->close();
				break;
			}

			ctx->clear();

			ctx->swapBuffers();	
			++frame;

			now = hires_time();
			if (((now - mark) / freq) > 1)
			{
				double elapsed = 
						static_cast<double>((now - mark) / (freq*1.0));

				double fps = frame / elapsed;
				LOG(VERBOSE) << "channel: fps:" 
						<< std::fixed << std::setprecision(3)
						<< fps;

				frame = 0;
				mark = hires_time();
			}

		}
	}

	gfx::shutdown();
	
	return 0;
}

void Channel::close()
{
	LOG(INFO) << "channel: " << id() << " closing ...";
	
	//_closed = true;
	
	uv_stop(uv_default_loop());
}

void Channel::iothread(void* arg) 
{
	Channel* self = reinterpret_cast<Channel*>(arg);
	if (!self){
		return;
	}

	uv_pipe_t pipe = {0};

	auto loop = uv_default_loop();

	if (uv_guess_handle(1) == UV_NAMED_PIPE)
	{
		trace::clearWriters();

		uv_pipe_init(loop, &pipe, 0);
		uv_pipe_open(&pipe, 1);
		uv_unref((uv_handle_t*)&pipe);
		
      auto writer = 
            std::make_shared<trace::UvWriter>((uv_stream_t*)&pipe);
		trace::addWriter(":pipe", writer);
	}

	// signal the thread is up and running
	uv_cond_signal(&self->_isReady);

	uv_idle_t idler;
	uv_idle_init(loop, &idler);
	uv_idle_start(&idler, wait_for_a_while);

	uv_run(loop, UV_RUN_DEFAULT);
}

#include "server.h"
#include "channel.h"

#include "pipe_trace.h"

#include "stopwatch.h"


#ifdef _WIN32
#include "window_win.h"
#else
#include "window_rpi.h"
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
#ifdef _WIN32
	::Sleep(10);
#endif
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

#ifdef _WIN32
		auto window = win::Window::create(channel);

		ctx = gfx::Context::create(window->handle());

		window->show();
		window->update();
#else

		auto window = rpi::Window::create();

		ctx = gfx::Context::create(window->handle());

#endif
		

		LOG(DEBUG) << "channel: staring rendering loop ...";

		uint32_t frame = 0;

		int64_t freq = hires_frequency();
		int64_t now, mark = hires_time();

		for (;;)
		{
#ifdef _WIN32
			if (window->pump() < 0)
			{
				channel->close();
				break;
			}
#endif

			//::Sleep(1);	

			ctx->clear();

			ctx->swapBuffers();	
			++frame;

			now = hires_time();
			if (((now - mark) / freq) > 1)
			{
				double elapsed = 
						static_cast<double>((now - mark) / (freq*1.0));

				double fps = frame / elapsed;
				LOG(VERBOSE) << "channel: " 
						<< std::fixed << std::setprecision(2)
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

	uv_pipe_t stdout_pipe = {0};

	auto loop = uv_default_loop();//uv_loop_new();

	if (uv_guess_handle(1) == UV_NAMED_PIPE)
	{
		trace::clearWriters();

		uv_pipe_init(loop, &stdout_pipe, 0);
		uv_pipe_open(&stdout_pipe, 1);
		uv_unref((uv_handle_t*)&stdout_pipe);
		
		auto writer = std::make_shared<trace::PipeWriter>(&stdout_pipe);
		trace::addWriter(":pipe", writer);
	}

	// signal the thread is up and running
	uv_cond_signal(&self->_isReady);

	uv_idle_t idler;
	uv_idle_init(loop, &idler);
	uv_idle_start(&idler, wait_for_a_while);

	uv_run(loop, UV_RUN_DEFAULT);

	//uv_loop_delete(loop);
}

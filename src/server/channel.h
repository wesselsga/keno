#pragma once

namespace win {
	class Window;
};

class Server;

class Channel
{
	struct _priv { };
public:
	explicit Channel(const _priv&, const std::string&);

	virtual ~Channel();
	
	static int32_t run(const std::string&);
	
	const std::string& id() const { return _id; }

protected:

private:
	Channel()=delete;
	Channel(const Channel&)=delete;
	const Channel& operator=(const Channel&) = delete;

	friend class win::Window;
	void close();

	static void iothread(void*);

	uv_thread_t _iothread;
	uv_mutex_t  _lock;
	uv_cond_t   _isReady;

	const std::weak_ptr<Server> _server;
	const std::string _id;
	std::shared_ptr<win::Window> _window;

};
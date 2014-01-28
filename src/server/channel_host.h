#pragma once

class ChannelHost
{
	struct _priv { };
public:
	explicit ChannelHost(const _priv&,
		const std::weak_ptr<Server>&,
		const std::string&);

	virtual ~ChannelHost();
	
	static std::shared_ptr<ChannelHost> create(
						uv_loop_t*,
						const std::weak_ptr<Server>&,
						const std::string&);
	
	const std::string& id() const { return _id; }
	
protected:

private:
	ChannelHost()=delete;
	ChannelHost(const ChannelHost&)=delete;
	const ChannelHost& operator=(const ChannelHost&) = delete;

	static void onExit(uv_process_t*, int64_t, int);
	
	std::string _id;
	const std::weak_ptr<Server> _server;

	uv_process_t* _process;
	uv_process_options_t _options;
	uv_pipe_t* _out;

};
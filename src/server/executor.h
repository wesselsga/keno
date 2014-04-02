#pragma once

class Executor
{
	struct _priv { };
public:
	explicit Executor(const _priv&,v8::Isolate*);

	virtual ~Executor();
	
	static std::shared_ptr<Executor> create();
	
	std::string version() const;

   void eval(const std::string&);
	
protected:

private:
	Executor()=delete;
	Executor(const Executor&)=delete;
	const Executor& operator=(const Executor&) = delete;

   v8::Handle<v8::Object> wrapLog();


	v8::Isolate* _isolate;
   
   v8::Handle<v8::Context> _context;
};
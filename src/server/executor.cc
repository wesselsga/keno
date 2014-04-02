#include "server.h"
#include "executor.h"



class JsLog
{
public:

   static v8::Handle<v8::ObjectTemplate> makeTemplate(v8::Isolate* iso)
   {
      v8::EscapableHandleScope scope(iso);
      v8::Local<v8::ObjectTemplate> templ = v8::ObjectTemplate::New(iso);
      templ->SetInternalFieldCount(1);

      templ->Set(v8::String::NewFromUtf8(iso, "error") , 
               v8::FunctionTemplate::New(iso, JsLog::error), v8::ReadOnly);
      templ->Set(v8::String::NewFromUtf8(iso, "warn") , 
               v8::FunctionTemplate::New(iso, JsLog::warn), v8::ReadOnly);
      templ->Set(v8::String::NewFromUtf8(iso, "info") , 
               v8::FunctionTemplate::New(iso, JsLog::info), v8::ReadOnly);
      templ->Set(v8::String::NewFromUtf8(iso, "verbose") , 
               v8::FunctionTemplate::New(iso, JsLog::verbose), v8::ReadOnly);
      templ->Set(v8::String::NewFromUtf8(iso, "debug") , 
               v8::FunctionTemplate::New(iso, JsLog::debug), v8::ReadOnly);
            
      return scope.Escape(templ);
   }

public:

   static void error(const v8::FunctionCallbackInfo<v8::Value>& args) 
   {
      LOG(ERR) << join(args);
   }

   static void warn(const v8::FunctionCallbackInfo<v8::Value>& args) 
   {
      LOG(WARN) << join(args);
   }

   static void info(const v8::FunctionCallbackInfo<v8::Value>& args) 
   {
      LOG(INFO) << join(args);
   }

   static void verbose(const v8::FunctionCallbackInfo<v8::Value>& args) 
   {
      LOG(VERBOSE) << join(args);
   }

   static void debug(const v8::FunctionCallbackInfo<v8::Value>& args) 
   {
      LOG(DEBUG) << join(args);
   }

   static std::string join(const v8::FunctionCallbackInfo<v8::Value>& args)
   {
      std::ostringstream ss;      
      for (auto i = 0; i < args.Length(); ++i)
      {
         v8::HandleScope scope(args.GetIsolate());
         if (i) {
            ss << " ";
         }         
         v8::String::Utf8Value str(args[i]);
         ss << ((*str) ? (*str) : "#ERR");
      }
      return ss.str();
   }

};



Executor::Executor(const _priv&, v8::Isolate* iso)
   : _isolate(iso) 
{
}
   
Executor::~Executor()
{
}

std::shared_ptr<Executor> Executor::create()
{
   // get the default Isolate created at startup.
   v8::Isolate* iso = v8::Isolate::GetCurrent();

   std::shared_ptr<Executor> exec(
         std::make_shared<Executor>(_priv{}, iso));

   // Create a stack-allocated handle scope.
   v8::HandleScope handle_scope(iso);

   v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(iso);
  
   v8::Handle<v8::Context> context = v8::Context::New(iso, NULL, global);
   
   // Enter the context for compiling and running the hello world script.
   v8::Context::Scope context_scope(context);

   v8::Handle<v8::ObjectTemplate> templ = JsLog::makeTemplate(iso);

      //Create an instance of the js object 
   v8::Handle<v8::Object> object = templ->NewInstance();
   
   v8::Handle<v8::External> ptr = v8::External::New(iso, nullptr);
   object->SetInternalField(0, ptr);
   
   context->Global()->Set(v8::String::NewFromUtf8(iso, "log"), object);

   exec->eval("log.error('hey from javascript');");

   return exec;
}

std::string Executor::version() const
{
   auto p = v8::V8::GetVersion();
   return std::string(p ? p : "n/a");
}

void Executor::eval(const std::string& expr)
{
   v8::Handle<v8::String> source = 
         v8::String::NewFromUtf8(_isolate, expr.c_str());
  
   // Compile the source code.
   v8::Handle<v8::Script> script = v8::Script::Compile(source);
  
   // Run the script to get the result.
   v8::Handle<v8::Value> result = script->Run();
  
   // Convert the result to an UTF8 string and print it.
   //v8::String::Utf8Value utf8(result);
   //printf("%s\n", *utf8);
}

v8::Handle<v8::Object> Executor::wrapLog()
{
   v8::EscapableHandleScope scope(_isolate);

   //This will persist when we leave the handle scope,
   //meaning that the actual c++ instance is preserved
   //v8::Persistent<v8::ObjectTemplate> class_template;

   v8::Handle<v8::ObjectTemplate> log_template = JsLog::makeTemplate(_isolate);
   //v8::Persistent<v8::ObjectTemplate> log_template;
   //log_template.Reset(_isolate, JsLog::makeTemplate(_isolate));
   
   v8::Handle<v8::ObjectTemplate> class_template =
         v8::Local<v8::ObjectTemplate>::New(_isolate, log_template);

   //This is so we can store a c++ object on it
   //raw_template->SetInternalFieldCount(1);

   //Create the actual template object, 
   //v8::Persistent<v8::ObjectTemplate> class_template = 
    //  v8::Persistent<v8::ObjectTemplate>::New(_isolate,raw_template);

   //Create an instance of the js object 
   v8::Local<v8::Object> result = class_template->NewInstance();

   //Create a wrapper for the c++ instance
   v8::Handle<v8::External> ptr = v8::External::New(_isolate, new JsLog()); 

   //Store the 'external c++ pointer' inside the JS object
   result->SetInternalField(0 , ptr);

   //Return the JS object representing this class
   return scope.Escape(result);
}
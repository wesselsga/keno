#include "server.h"
#include "config.h"

class JsonConfig : public Config
{
public:
   JsonConfig(){}
   ~JsonConfig(){}
   
   std::vector<std::shared_ptr<ChannelBlock const>> const& channels(){ return _channels; }

   std::shared_ptr<ChannelBlock const> getChannel(std::string const&);

   bool parse(Path const&);

private:

   bool addChannel(v8::Isolate*, v8::Local<v8::Object> const&);
   
   std::string getString(v8::Isolate*, v8::Local<v8::Object> const&, 
         std::string const&, std::string const&);

   int32_t getInt32(v8::Isolate*, v8::Local<v8::Object> const&, 
         std::string const&, int32_t const);
   uint32_t getUint32(v8::Isolate*, v8::Local<v8::Object> const&, 
         std::string const&, uint32_t const);

   std::vector<std::shared_ptr<ChannelBlock const>> _channels;

};


//
// open the default configuration file
//
std::shared_ptr<Config> Config::open()
{
   auto path = Path::home();
   path = path.append(PRODUCT);

   std::string file(PRODUCT);
   file.append(".conf");
   path = path.append(file);

   std::shared_ptr<JsonConfig> cfg = std::make_shared<JsonConfig>();
   if (cfg->parse(path)){
      return cfg;
   }
      
   return nullptr;
}

bool JsonConfig::parse(Path const& filename)
{
   std::ifstream in(filename.str());
   if (!in.is_open())
   {
      LOG(ERR) << "config: could not open " << filename.str();
      return false;
   }

   std::istreambuf_iterator<char> eos;
   std::string text(std::istreambuf_iterator<char>(in), eos);   

   v8::Isolate* iso = v8::Isolate::GetCurrent();
   v8::HandleScope handle_scope(iso);

   v8::Handle<v8::ObjectTemplate> global = v8::ObjectTemplate::New(iso);
  
   v8::Handle<v8::Context> context = v8::Context::New(iso, NULL, global);
   
   v8::Context::Scope scope(context);

   auto input = v8::String::NewFromUtf8(iso, text.c_str());

   v8::Local<v8::Value> result = v8::JSON::Parse(input);

   if (result.IsEmpty()) 
   {
      LOG(ERR) << "config: error parsing JSON";
      return false;
   }

   auto obj = result->ToObject();
   if (!obj.IsEmpty())
   {
      auto channels = obj->Get(v8::String::NewFromUtf8(iso, "channels"));
      if (!channels.IsEmpty() && channels->IsArray())
      {
         v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(channels);
         for (uint32_t n = 0; n < array->Length(); ++n)
         {
            auto c = array->Get(n);
            if (!c.IsEmpty() && c->IsObject()){
               addChannel(iso, c->ToObject());
            }
         }
      }

   }
   
   return true;
}

std::string JsonConfig::getString(v8::Isolate* iso, 
      v8::Local<v8::Object> const& obj, std::string const& prop, std::string const& defval)
{
   if (!iso || obj.IsEmpty()){
      return defval;
   }

   auto v = obj->Get(v8::String::NewFromUtf8(iso, prop.c_str()));
   if (v.IsEmpty() || !v->IsString()){
      return defval;
   }

   v8::String::Utf8Value utf8(v->ToString());

   return std::string(*utf8);
}

int32_t JsonConfig::getInt32(v8::Isolate* iso, 
      v8::Local<v8::Object> const& obj, std::string const& prop, int32_t const defval)
{
   if (!iso || obj.IsEmpty()){
      return defval;
   }

   auto v = obj->Get(v8::String::NewFromUtf8(iso, prop.c_str()));
   if (v.IsEmpty() || !v->IsNumber()){
      return defval;
   }

   return v->Int32Value();
}

uint32_t JsonConfig::getUint32(v8::Isolate* iso, 
      v8::Local<v8::Object> const& obj, std::string const& prop, uint32_t const defval)
{
   if (!iso || obj.IsEmpty()){
      return defval;
   }

   auto v = obj->Get(v8::String::NewFromUtf8(iso, prop.c_str()));
   if (v.IsEmpty() || !v->IsNumber()){
      return defval;
   }

   return v->Uint32Value();
}

bool JsonConfig::addChannel(v8::Isolate* iso, v8::Local<v8::Object> const& obj)
{
   if (obj.IsEmpty()){
      return false;
   }

   auto name = getString(iso, obj, "name", "");
   if (name.empty()){
      return false;
   }

   auto w = getUint32(iso, obj, "width", 0);
   auto h = getUint32(iso, obj, "height", 0);
   
   _channels.push_back(std::make_shared<ChannelBlock>(name, w, h));
   return true;
}

//
// helper to select a channel block by name
//
std::shared_ptr<ChannelBlock const> JsonConfig::getChannel(std::string const& name)
{
   for (auto c : _channels)
   {
      if (c && c->name() == name){
         return c;
      }
   }
   return nullptr;
}
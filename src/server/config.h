#pragma once


class ChannelBlock
{
public:
   ChannelBlock():_width(0), _height(0) {}
   ChannelBlock(std::string& name, uint32_t w, uint32_t h) 
      : _name(name), _width(w), _height(h)
   {   
   }

   std::string name() const { return _name; }

   uint32_t width() const { return _width; }
   uint32_t height() const { return _height; }

private:

   std::string _name;
   uint32_t _width;
   uint32_t _height;
};


class Config
{
public:
   virtual ~Config(){}

   static std::shared_ptr<Config> open();

   virtual std::vector<std::shared_ptr<ChannelBlock const>> const& channels()=0;

   virtual std::shared_ptr<ChannelBlock const> getChannel(std::string const&)=0;

protected:
   Config(){}

};

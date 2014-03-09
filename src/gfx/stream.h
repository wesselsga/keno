#pragma once

namespace gfx {

class Stream
{
public:
   virtual ~Stream();

   virtual uint32_t width() const =0;
   virtual uint32_t height() const =0;
   virtual uint32_t texture()=0;

protected:
   Stream();

};

};

#include "core.h"


Uuid::Uuid() 
   : _data1(0), _data2(0),_data3(0),_data4(0)
{
}

Uuid::Uuid(uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4)
   : _data1(d1), _data2(d2),_data3(d3),_data4(d4)
{
}

Uuid::Uuid(const Uuid& copy) 
   : _data1(copy._data1), 
      _data2(copy._data2), 
      _data3(copy._data3), 
      _data4(copy._data4)
{
}

std::shared_ptr<Uuid> Uuid::parse(const std::string&)
{
   return nullptr;
}

std::ostream& operator<<(std::ostream& out, const Uuid& uuid)
{
   out << std::setfill('0') << std::hex 
      << std::setw(8) << uuid._data1
      << "-" << std::setw(4) 
      << static_cast<uint16_t>((uuid._data2 >> 16) & 0x0000FFFF)
      << "-" << std::setw(4) 
      << static_cast<uint16_t>(uuid._data2 & 0x0000FFFF)
      << "-" << std::setw(4) 
      << static_cast<uint16_t>((uuid._data3 >> 16) & 0x0000FFFF)
      << "-" << std::setw(4) 
      << static_cast<uint16_t>(uuid._data3 & 0x0000FFFF)
      << std::setw(8) << uuid._data4;

   return out;
}
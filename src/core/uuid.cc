#include "core.h"

#include <random>

std::random_device __rng_dev;
std::mt19937_64    __rng(__rng_dev());
std::mutex         __rng_lock;

uint64_t random()
{
   std::lock_guard<std::mutex>guard(__rng_lock);
   return __rng();
}


Uuid::Uuid() : Uuid(0,0,0,0)
{
}

Uuid::Uuid(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3)
   : _data0(d0), _data1(d1),_data2(d2),_data3(d3)
{
}

Uuid::Uuid(const Uuid& copy) 
   : _data0(copy._data0), _data1(copy._data1), 
      _data2(copy._data2), _data3(copy._data3)
{
}

union block
{
    uint8_t  b[4];
    uint32_t u;
};

Uuid Uuid::make()
{
   std::uniform_int_distribution<> dis(0, 0xff);
   std::lock_guard<std::mutex>guard(__rng_lock);
   
   union block d0, d1, d2, d3;

   //0x00112233

   d0.b[0] = dis(__rng);
   d0.b[1] = dis(__rng);
   d0.b[2] = dis(__rng);
   d0.b[3] = dis(__rng);

   //d0.b[0] = dis(__rng);
   //d0.b[1] = dis(__rng);
   //d0.b[2] = dis(__rng);
   //d0.b[3] = dis(__rng);

   d1.b[0] = dis(__rng);
   d1.b[1] = 0x40 | (dis(__rng) & 0x0f);
   d1.b[2] = dis(__rng);
   d1.b[3] = dis(__rng);

   d2.b[0] = dis(__rng);
   d2.b[1] = dis(__rng);
   d2.b[2] = dis(__rng);
   d2.b[3] = 0x80 | (dis(__rng) & 0x3f);
   
   d3.b[0] = dis(__rng);
   d3.b[1] = dis(__rng);
   d3.b[2] = dis(__rng);
   d3.b[3] = dis(__rng);

   return Uuid(d0.u,d1.u,d2.u,d3.u);
}

std::shared_ptr<Uuid> Uuid::parse(const std::string&)
{
   return nullptr;
}

std::ostream& operator<<(std::ostream& out, const Uuid& uuid)
{
   out << std::setfill('0') << std::hex 
      << std::setw(8) << uuid._data0
      << "-" << std::setw(4) 
      << static_cast<uint16_t>((uuid._data1 >> 16) & 0x0000FFFF)
      << "-" << std::setw(4) 
      << static_cast<uint16_t>(uuid._data1 & 0x0000FFFF)
      << "-" << std::setw(4) 
      << static_cast<uint16_t>((uuid._data2 >> 16) & 0x0000FFFF)
      << "-" << std::setw(4) 
      << static_cast<uint16_t>(uuid._data2 & 0x0000FFFF)
      << std::setw(8) << uuid._data3;

   return out;
}
#include "core.h"

#include <random>

std::random_device __rng_dev;
std::mt19937_64    __rng(__rng_dev());
std::mutex         __rng_lock;

/*uint64_t random()
{
   std::lock_guard<std::mutex>guard(__rng_lock);
   return __rng();
}*/

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

bool Uuid::operator==(const Uuid& rhs) const
{
   if ((_data0 == rhs._data0) && (_data1 == rhs._data1) &&
      (_data2 == rhs._data2) && (_data3 == rhs._data3))
   {
      return true;
   }
   return false;
}

bool Uuid::operator!=(const Uuid& rhs) const
{
   if ((_data0 != rhs._data0) || (_data1 != rhs._data1) ||
      (_data2 != rhs._data2) && (_data3 != rhs._data3))
   {
      return true;
   }
   return false;
}

bool Uuid::empty() const
{
   if (!_data0 && !_data1 && !_data2 && !_data3){
      return true;
   }
   return false;
}

union block
{
    uint8_t  b[4];
    uint32_t u;
};

Uuid Uuid::make()
{
   // RFC4122 version 4

   std::uniform_int_distribution<> dis(0, 0xff);
   std::lock_guard<std::mutex>guard(__rng_lock);
   
   union block d0, d1, d2, d3;

   d0.b[0] = dis(__rng);
   d0.b[1] = dis(__rng);
   d0.b[2] = dis(__rng);
   d0.b[3] = dis(__rng);
   
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

inline uint8_t nibble(char c)
{
	if (c >= '0' && c <= '9') return c - '0';
   if (c >= 'a' && c <= 'f') return c - 'a' + 10;
   if (c >= 'A' && c <= 'F') return c - 'A' + 10; 
   return 255;
}

uint8_t read_byte(const char* p)
{
   if (!(*p) || !(*(p+1))){
      throw std::invalid_argument("invalid format");
   }   

   char c0 = *p;
   char c1 = *(p + 1);
   if (!isxdigit(c0) || !isxdigit(c1)) {
      throw std::invalid_argument("invalid uuid format");
   }

   return (nibble(c0) << 4) | nibble(c1) ;
}

//
// Example:
// ECBD63F4-2125-48C7-BEBD-0BB95DC982DD
//
// or,
// ECBD63F4212548C7BEBD0BB95DC982DD
//
Uuid Uuid::parse(const char* input)
{
   if (!input){
      return Uuid();
   }
   
   const char* p = input;
   union block d0, d1, d2, d3;
   
   d0.b[3] = read_byte(p); p += 2;
   d0.b[2] = read_byte(p); p += 2;
   d0.b[1] = read_byte(p); p += 2;
   d0.b[0] = read_byte(p); p += 2;
   if (*p == '-'){ p++; }
   
   d1.b[3] = read_byte(p); p += 2;
   d1.b[2] = read_byte(p); p += 2;
   if (*p == '-'){ p++; }

   d1.b[1] = read_byte(p); p += 2;
   d1.b[0] = read_byte(p); p += 2;
   if (*p == '-'){ p++; }
   
   d2.b[3] = read_byte(p); p += 2;
   d2.b[2] = read_byte(p); p += 2;
   if (*p == '-'){ p++; }

   d2.b[1] = read_byte(p); p += 2;
   d2.b[0] = read_byte(p); p += 2;   
   d3.b[3] = read_byte(p); p += 2;
   d3.b[2] = read_byte(p); p += 2;
   d3.b[1] = read_byte(p); p += 2;
   d3.b[0] = read_byte(p); p += 2;

   return Uuid(d0.u, d1.u, d2.u, d3.u);
}

Uuid Uuid::parse(const std::string& input)
{
   return parse(input.c_str());   
}

std::ostream& operator<<(std::ostream& out, const Uuid& uuid)
{
   out << std::setfill('0') << std::hex << std::uppercase
      << std::setw(8) << uuid._data0
      << "-" << std::setw(4) 
      << static_cast<uint16_t>((uuid._data1 >> 16) & 0x0000FFFF)
      << "-" << std::setw(4) 
      << static_cast<uint16_t>(uuid._data1 & 0x0000FFFF)
      << "-" << std::setw(4) 
      << static_cast<uint16_t>((uuid._data2 >> 16) & 0x0000FFFF)
      << "-" << std::setw(4) 
      << static_cast<uint16_t>(uuid._data2 & 0x0000FFFF)
      << std::setw(8) << uuid._data3
      << std::dec;

   return out;
}
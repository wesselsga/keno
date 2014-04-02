#pragma once

class Uuid
{
public:
   Uuid();
   Uuid(uint32_t,uint32_t,uint32_t,uint32_t);
   Uuid(const Uuid&);

   static Uuid make();
   static Uuid parse(const char*);
   static Uuid parse(const std::string&);
   
   uint32_t data0() const { return _data0; }
   uint32_t data1() const { return _data1; }
   uint32_t data2() const { return _data2; }
   uint32_t data3() const { return _data3; }

   bool operator==(const Uuid&) const;
   bool operator!=(const Uuid&) const;

   bool empty() const;

private:
   Uuid& operator=(const Uuid&) = delete;  

   friend std::ostream& operator<<(std::ostream&, const Uuid&);

   const uint32_t _data0;
   const uint32_t _data1;
   const uint32_t _data2;
   const uint32_t _data3;

};

std::ostream& operator<<(std::ostream&, const Uuid&);

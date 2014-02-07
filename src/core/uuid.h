#pragma once


class Uuid
{
public:
   Uuid();
   Uuid(uint32_t,uint32_t,uint32_t,uint32_t);
   Uuid(const Uuid&);

   static std::shared_ptr<Uuid> parse(const std::string&);

   uint32_t data1() const { return _data1; }
   uint32_t data2() const { return _data2; }
   uint32_t data3() const { return _data3; }
   uint32_t data4() const { return _data4; }

private:
   Uuid& operator=(const Uuid&){ return *this; }   

   friend std::ostream& operator<<(std::ostream&, const Uuid&);

   const uint32_t _data1;
   const uint32_t _data2;
   const uint32_t _data3;
   const uint32_t _data4;

};

std::ostream& operator<<(std::ostream&, const Uuid&);

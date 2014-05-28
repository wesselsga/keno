#pragma once

#include <vector>

class Path
{
public:
   Path(){}
   Path(const std::string& s) : _str(s) { }
	Path(const Path& rhs){ assign(rhs); }
   ~Path(){}

   static Path home();

   Path& operator=(const Path& rhs)
	{
		if (this != &rhs){
			assign(rhs);
		}
		return *this;
	}

   Path append(const std::string&) const;
   bool mkdir() const;
   bool exists() const;
   bool is_dir() const;

   bool list(std::vector<std::string>&) const;

	const std::string& str() const { return _str; }
	const char* c_str() const { return _str.c_str(); }
   
private:
   char separator() const;
	
   void assign(const Path& rhs)
   {
      _str = rhs._str;
   }

   std::string _str;
};

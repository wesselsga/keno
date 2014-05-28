#include "core.h"


Path Path::append(const std::string& part) const
{
   std::string tmp(str());
   if (!tmp.empty()) 
	{
		size_t n = tmp.size() - 1;
		if (tmp[n] != separator())
			tmp += separator();
	}
	tmp.append(part);
   return Path(tmp);
}
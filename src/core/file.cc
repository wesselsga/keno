#include "core.h"

Path::Path()
{
}

Path::Path(const std::string&)
{
}

Path::~Path()
{
}

void Path::assign(const Path& rhs)
{
	_str = rhs._str;
}
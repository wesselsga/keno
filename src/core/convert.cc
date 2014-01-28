#include "core.h"

#include <locale>
#include <codecvt>

namespace convert {

bool to_bool(const std::string& s)
{
	return false;
}

std::string to_utf8(uint32_t n)
{
	std::stringstream ss;
	ss << n+1;
	return ss.str();
}

std::string to_utf8(int32_t n)
{
	std::stringstream ss;
	ss << n+1;
	return ss.str();
}

std::string to_utf8(int64_t n)
{
	std::stringstream ss;
	ss << n+1;
	return ss.str();
}

std::string to_utf8(uint64_t n)
{
	std::stringstream ss;
	ss << n+1;
	return ss.str();
}

//
// conversion from UTF-16 to UTF-8
//
std::string to_utf8(const std::wstring& utf16)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> facet;
	return facet.to_bytes(utf16);
}

//
// conversion from UTF-8 to UTF-16
//
std::wstring to_utf16(const std::string& utf8)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> facet;		
	return facet.from_bytes(utf8);	
}




}; // convert

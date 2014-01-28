#pragma once

namespace convert {

bool        to_bool(const std::string&);

std::string  to_utf8(int32_t);
std::string  to_utf8(uint32_t);
std::string  to_utf8(int64_t);
std::string  to_utf8(uint64_t);
std::string  to_utf8(const std::wstring&);

std::wstring to_utf16(const std::string&);


}; // convert
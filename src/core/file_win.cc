#include "core.h"

#include <shlobj.h>

char Path::separator() const
{
   return '\\';
}

Path Path::home()
{
   wchar_t psz[512] = {0};
   HRESULT hr = SHGetFolderPath(0, CSIDL_LOCAL_APPDATA, 0, 0, psz);
   if (SUCCEEDED(hr)){
      return Path(convert::to_utf8(psz));     
   }
   throw std::runtime_error("unknown location");
}

bool Path::exists() const
{
   std::wstring ws = convert::to_utf16(str());
   return (::GetFileAttributes(ws.c_str()) != INVALID_FILE_ATTRIBUTES);
}

bool Path::is_dir() const
{
   std::wstring ws = convert::to_utf16(str());
   uint32_t flags = ::GetFileAttributes(ws.c_str());
   if (flags != INVALID_FILE_ATTRIBUTES){
      return (flags & FILE_ATTRIBUTE_DIRECTORY);
   }
   return false;
}

bool Path::mkdir() const
{
   std::wstring ws=convert::to_utf16(_str);
   if (::CreateDirectory(ws.c_str(), 0)) {
      return true;
   }
   return false;
}

bool Path::list(std::vector<std::string>& files) const
{
   if (!is_dir()){
      return false;
   }

   // ensure path ends with a '\'
   std::wstring ws=convert::to_utf16(_str);
   if (!ws.empty() && ws.at(ws.size()-1) != separator()){
      ws.append(1, separator());
   }
   ws.append(L"*");

   WIN32_FIND_DATA fd;
   HANDLE find = ::FindFirstFile(ws.c_str(), &fd);
   if (!find || find == INVALID_HANDLE_VALUE){
      return false;
   }

   do
   {
      if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         if (!wcscmp(fd.cFileName, L".") || !wcscmp(fd.cFileName, L"..")){
            continue;
         }
      }
      files.push_back(convert::to_utf8(fd.cFileName));
   
   }while (::FindNextFile(find, &fd));

   ::FindClose(find);

   return true;
}
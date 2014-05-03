#include "core.h"

#ifdef _WIN32
#include <shlobj.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#endif

// well-known folder locations on a system
enum class FolderId
{
   UserHome
};

Path locate(enum class FolderId);


Path::Path()
{
}

Path::Path(const std::string& s) : _str(s)
{
}

Path::~Path()
{
}

void Path::assign(const Path& rhs)
{
	_str = rhs._str;
}

Path Path::home()
{
   return locate(FolderId::UserHome);
}

char Path::separator() const
{
#ifdef _WIN32
   return '\\';
#else
   return '/';
#endif
}

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

bool Path::mkdir() const
{
#ifdef _WIN32

   std::wstring ws=convert::to_utf16(_str);
   if (::CreateDirectory(ws.c_str(), 0)) {
      return true;
   }

#else

   if (!mkdir(c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
      return true;
   }

#endif
   
   return false;
}

#ifdef _WIN32

Path locate(FolderId id)
{
   wchar_t psz[512] = {0};
   HRESULT hr;

   if (id == FolderId::UserHome)
   {
      hr = SHGetFolderPath(0, CSIDL_LOCAL_APPDATA, 0, 0, psz);
      if (SUCCEEDED(hr)){
         return Path(convert::to_utf8(psz));     
      }      
   }

   throw std::bad_exception("?");   
}

#else

Path locate(FolderId id)
{
   if (id == FolderId::UserHome)
   {
      char* dir = getenv("HOME");
      if (dir) {
         return Path(dir);
      }
      uid_t uid = getuid();
      struct passwd *pw = getpwuid(uid);
      if (pw) {
         return Path(pw->pw_dir);
      }
   }

   throw std::bad_exception("?");
}   

#endif
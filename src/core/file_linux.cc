#include "core.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>

Path Path::home()
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

   throw std::runtime_error("unknown location");
}

bool Path::exists() const
{
   struct stat st;
   return (stat(c_str(), &st) == 0);
}

bool Path::is_dir() const
{
   struct stat st;
   if (stat(c_str(), &st) == 0) {
      return ((st.st_mode & S_IFDIR) != 0);
   }
   return false;
}

char Path::separator() const
{
   return '/';
}

bool Path::mkdir() const
{
   if (!::mkdir(c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
      return true;
   }
   return false;
}

bool Path::list(std::vector<std::string>& files) const
{
   DIR* dpdf = opendir(c_str());
   if (dpdf)
   {
      struct dirent *epdf;
      while (epdf = readdir(dpdf))
      {
         if (strcmp(epdf->d_name, ".") && strcmp(epdf->d_name, "..")) {
            files.push_back(epdf->d_name);
         }
      }
      return true;
   }
   return false;
}

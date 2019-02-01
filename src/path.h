#ifndef SMW_PATH_HEADER
#define SMW_PATH_HEADER
#include <string>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

extern char SMW_Root_Data_Dir[PATH_MAX + 2];

/* Call Initialize_Paths() when your application launches */

bool File_Exists (const std::string fileName);
/* All filenames must go through this door */
#define convertPathC(s) convertPath(s).c_str()
const std::string convertPath(const std::string& source);

#define convertPathCP(s, p) convertPath(s, p).c_str()
const std::string convertPath(const std::string& source, const std::string& pack);

const std::string getDirectorySeperator();
const std::string convertPartialPath(const std::string & source);
const std::string getFileFromPath(const std::string &path);

//#undef convertPathC
//#define convertPathC(s) s
//#define convertPath(s) s 

#ifdef __MACOSX__
    void Initialize_Paths();
#else
#  define Initialize_Paths() ;
#endif

#endif


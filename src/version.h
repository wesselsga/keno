#pragma once

#define STRINGIFY2(s) #s
#define STRINGIFY(s) STRINGIFY2(s)

#define PRODUCT      "keno"
#define COPYRIGHT    ""

#define VERSION_MAJOR    0
#define VERSION_MINOR    9
#define VERSION_REVISION 0
#define VERSION_BUILD    0

#define VERSION_FULL	    VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
#define VERSION_STR      STRINGIFY(VERSION_MAJOR)        \
                                    "." STRINGIFY(VERSION_MINOR)    \
                                    "." STRINGIFY(VERSION_REVISION) \
                                    "." STRINGIFY(VERSION_BUILD)    \

// windows platform targeting
#ifdef _WIN32
#ifndef WINVER
#define WINVER 0x0601 // win7
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT WINVER
#endif
#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif
#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif
#endif


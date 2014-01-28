#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "../../deps/libuv/uv.h"

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
#include <windows.h>
#endif

#include "convert.h"
#include "file.h"
#include "trace.h"

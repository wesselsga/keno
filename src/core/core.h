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
#include <fstream>
#include <iomanip>
#include <sstream>
#include <exception>

#include <mutex>

#include <uv/uv.h>
#include <v8/v8.h>

#include "../version.h"
#ifdef _WIN32
#include <windows.h>
#endif

#include "uuid.h"
#include "convert.h"
#include "file.h"
#include "trace.h"

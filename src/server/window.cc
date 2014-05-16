#include "server.h"
#include "window.h"


#ifdef _WIN32
#include "window_win.h"
#else
#ifdef _RASPI
#include "window_raspi.h"
#else
#include "window_linux.h"
#endif
#endif



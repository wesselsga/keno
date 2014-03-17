#include "gfx.h"

#include <freeimage.h>

void gfx::startup()
{
	FreeImage_Initialise();
	LOG(DEBUG) << "gfx: using FreeImage " << FreeImage_GetVersion();
	
}

void gfx::shutdown()
{
	FreeImage_DeInitialise();
	
	LOG(DEBUG) << "gfx: shutdown.";	
}

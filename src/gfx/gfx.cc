#include "gfx.h"

#include <FreeImage.h>

void gfx::startup()
{
	FreeImage_Initialise();
	LOG(DEBUG) << "gfx: using FreeImage " << FreeImage_GetVersion();
	
}

void gfx::shutdown()
{
	FreeImage_Initialise();
	
	LOG(DEBUG) << "gfx: shutdown.";	
}
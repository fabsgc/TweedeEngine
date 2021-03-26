#pragma once

#include "TeGLRenderAPIPrerequisites.h"

namespace te
{
    class Win32GLSupport;
    class Win32Context;
    class Win32RenderWindow;

    /**	Retrieves last Windows API error and returns a description of it. */
    String TranslateWGLError();
}

#pragma once

#include <assert.h>

// Platform-specific stuff
#include "Prerequisites/TePlatformDefines.h"

#if TE_COMPILER == TE_COMPILER_MSVC
// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning (disable: 4251)

// disable: 'X' Function call with parameters that may be unsafe
#   pragma warning(disable: 4996)

// disable: usage of the macro - based offsetof pattern in constant expressions is non 
//- standard; use offsetof defined in the C++ standard library instead
#   pragma warning(disable: 4644)

// disable: decorated name length exceeded, name was truncated
// Happens with really long type names. Even fairly standard use
// of std::unordered_map with custom parameters, meaning I can't
// really do much to avoid it. It shouldn't effect execution
// but might cause problems if you compile library
// with one compiler and use it in another.
#   pragma warning(disable: 4503)

// disable: C++ exception handler used, but unwind semantics are not enabled
// We don't care about this as any exception is meant to crash the program.
#   pragma warning(disable: 4530)
#endif

// Windows Settings
#if TE_PLATFORM == TE_PLATFORM_WIN32
// Win32 compilers use _DEBUG for specifying debug builds.
// for MinGW, we set DEBUG
#   if defined(_DEBUG) || defined(DEBUG)
#       define TE_DEBUG_MODE 1
#   else
#       define TE_DEBUG_MODE 0
#   endif
#   
#   undef _WIN32_WINNT
#
#   if defined(TE_WIN_SDK_7)
#       pragma warning(push)
#       pragma warning( disable : 4005 )
#   elif defined(TE_WIN_SDK_8)
#       define WIN32_LEAN_AND_MEAN
#       define _WIN32_WINNT _WIN32_WINNT_WIN8
#   elif defined(TE_WIN_SDK_10)
#       define WIN32_LEAN_AND_MEAN
#       define _WIN32_WINNT _WIN32_WINNT_WIN10
#   endif
#endif

#if TE_PLATFORM == TE_PLATFORM_WIN32
#else
#   include <uuid/uuid.h>
#endif

// Linux/Apple Settings
#if TE_PLATFORM == TE_PLATFORM_LINUX
// A quick define to overcome different names for the same function
#   define stricmp strcasecmp

#   ifdef DEBUG
#       define TE_DEBUG_MODE 1
#   else
#       define TE_DEBUG_MODE 0
#   endif
#endif

namespace te
{
    static bool IsBigEndian()
    {
#if TE_ENDIAN == TE_ENDIAN_BIG
        return true;
#else
        return false;
#endif
    }
}

#include "Error/TeDebug.h"
#include "Error/TeError.h"
#include "Error/TeConsole.h"

#include "Prerequisites/TeTypes.h"
#include "Prerequisites/TeForwardDecl.h"

#include "Prerequisites/TeStdHeaders.h"

#include "String/TeString.h"

#include "Utility/TeUtility.h"

#include "Utility/TeUUID.h"

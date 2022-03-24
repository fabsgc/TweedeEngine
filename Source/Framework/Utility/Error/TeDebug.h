#pragma once

#include <iostream>
#include <fstream>
#include <cstring>

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
#   ifndef TE_DEBUG_FILE
#       define TE_DEBUG_FILE "Log/Debug.log"
#   endif

#if TE_PLATFORM == TE_PLATFORM_WIN32 && !defined __FILENAME__
#   define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif TE_PLATFORM == TE_PLATFORM_LINUX && !defined __FILENAME__
#   include <string.h>
#   define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#   ifndef TE_DEBUG
#   define TE_DEBUG(message)                                                                     \
        {                                                                                        \
            ::std::ofstream logFile(TE_DEBUG_FILE, ::std::ios_base::out | ::std::ios_base::app); \
                                                                                                 \
            logFile << "Log: " << __FILENAME__ << ":" << __LINE__ << ::std::endl;                \
            logFile << "Inside " << __FUNCTION__ << ::std::endl;                                 \
            logFile << "Date: " << __DATE__ << ::std::endl;                                      \
            logFile << "Time: " << __TIME__ << ::std::endl;                                      \
            logFile << "Message: " << message << ::std::endl;                                    \
            logFile << "############################################: " << ::std::endl;          \
                                                                                                 \
            ::std::cout << "Log: " << __FILENAME__ << ":" << __LINE__ << ::std::endl;            \
            ::std::cout << "Inside " << __FUNCTION__ << ::std::endl;                             \
            ::std::cout << "Message: " << message << ::std::endl;                                \
            ::std::cout << "############################################: " << ::std::endl;      \
        }
#   endif

#   ifndef TE_PRINT
#   define TE_PRINT(message)                                                                     \
        {                                                                                        \
            ::std::cout << message << ::std::endl;                                               \
        }
#   endif
#else 
#   ifndef TE_DEBUG
#   define TE_DEBUG(message) (void)0
#   endif
#   ifndef TE_PRINT
#   define TE_PRINT(message) (void)0
#   endif
#endif

#pragma once

#include "TeEngineConfig.h"


#if TE_DEBUG_MODE == 1
#   ifndef TE_DEBUG_FILE
#       define TE_DEBUG_FILE "Log/Debug.log"
#   endif

#   ifndef TE_DEBUG
#   define TE_DEBUG(message, file, line)                                                         \
        {                                                                                        \
            ::std::ofstream logFile(TE_DEBUG_FILE, ::std::ios_base::out | ::std::ios_base::app); \
                                                                                                 \
            logFile << "Log: " << file << ":" << line << std::endl;                              \
            logFile << "Inside " << __FUNCTION__ << std::endl;                                   \
            logFile << "Date: " << __DATE__ << std::endl;                                        \
            logFile << "Time: " << __TIME__ << std::endl;                                        \
            logFile << "Message: " << message << std::endl;                                      \
            logFile << "############################################: " << std::endl;            \
                                                                                                 \
            std::cout << "Log: " << file << ":" << line << std::endl;                            \
            std::cout << "Inside " << __FUNCTION__ << std::endl;                                 \
            std::cout << "Date: " << __DATE__ << std::endl;                                      \
            std::cout << "Time: " << __TIME__ << std::endl;                                      \
            std::cout << "Message: " << message << std::endl;                                    \
            std::cout << "############################################: " << std::endl;          \
        }                                                                                   
#   endif

#   ifndef TE_PRINT
#   define TE_PRINT(message)                                                                     \
        {                                                                                        \
            std::cout << message << std::endl;                                                   \
        }
#   endif
#else 
#   ifndef TE_DEBUG
#   define TE_DEBUG(message, file, line)
#   endif
#   ifndef TE_PRINT
#   define TE_PRINT(message, file, line)
#   endif
#endif
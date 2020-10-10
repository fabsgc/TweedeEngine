#pragma once

#include <iostream>
#include <fstream>

#if TE_DEBUG_MODE == 1
#   ifndef TE_ASSERT_ERROR_ASSERT
#   define TE_ASSERT_ERROR_ERROR(condition) assert((condition));
#   endif
#else
#   ifndef TE_ASSERT_ERROR_ASSERT
#   define TE_ASSERT_ERROR_ERROR(condition)
#   endif
#endif

#ifndef TE_ERROR_FILE
#   define TE_ERROR_FILE "Log/Errors.log"
#endif

#if TE_PLATFORM == TE_PLATFORM_WIN32 && !defined __FILENAME__
#   define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif TE_PLATFORM == TE_PLATFORM_LINUX && !defined __FILENAME__
#   define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#ifndef TE_ASSERT_ERROR
#define TE_ASSERT_ERROR(condition, error)                                                               \
    {                                                                                                   \
        if (!(condition))                                                                               \
        {                                                                                               \
            ::std::ofstream errorFile(TE_ERROR_FILE, ::std::ios_base::out | ::std::ios_base::app);      \
                                                                                                        \
            errorFile << "Assertion failed at " << __FILENAME__ << ":" << __LINE__ << ::std::endl;      \
            errorFile << "Inside " << __FUNCTION__ << ::std::endl;                                      \
            errorFile << "Date: " << __DATE__ << ::std::endl;                                           \
            errorFile << "Time: " << __TIME__ << ::std::endl;                                           \
            errorFile << "Error: " << error << ::std::endl;                                             \
            errorFile << "Condition: " << (condition) << ::std::endl;                                   \
            errorFile << "############################################: " << ::std::endl;               \
                                                                                                        \
            std::cout << "Assertion failed at " << __FILENAME__ << ":" << __LINE__ << ::std::endl;      \
            std::cout << "Inside " << __FUNCTION__ << std::endl;                                        \
            std::cout << "Message: " << error << std::endl;                                             \
            std::cout << "############################################: " << std::endl;                 \
            TE_ASSERT_ERROR_ERROR(condition);                                                           \
        }                                                                                               \
    }
#endif

#ifndef TE_ASSERT_ERROR_SHORT
#define TE_ASSERT_ERROR_SHORT(condition)                                                                \
    {                                                                                                   \
        if (!(condition))                                                                               \
        {                                                                                               \
            ::std::ofstream errorFile(TE_ERROR_FILE, ::std::ios_base::out | ::std::ios_base::app);      \
                                                                                                        \
            errorFile << "Assertion failed at " << __FILENAME__ << ":" << __LINE__;                     \
            errorFile << " inside " << __FUNCTION__ << ::std::endl;                                     \
            errorFile << "Date: " << __DATE__ << ::std::endl;                                           \
            errorFile << "Time: " << __TIME__ << ::std::endl;                                           \
            errorFile << "Condition: " << (condition);                                                  \
            errorFile << "############################################: " << ::std::endl;               \
                                                                                                        \
            std::cout << "Assertion failed at " << __FILENAME__ << ":" << __LINE__ << ::std::endl;      \
            std::cout << "Inside " << __FUNCTION__ << std::endl;                                        \
            std::cout << "Message: " << error << std::endl;                                             \
            std::cout << "############################################: " << std::endl;                 \
            TE_ASSERT_ERROR_ERROR(condition);                                                           \
        }                                                                                               \
    }
#endif

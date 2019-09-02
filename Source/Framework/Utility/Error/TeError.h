#pragma once

#include <iostream>
#include <fstream>

#if TE_DEBUG_MODE == 1
#   ifndef TE_ERROR_FILE
#       define TE_ERROR_FILE "Log/Errors.log"
#   endif

#   ifndef TE_ASSERT_ERROR
#   define TE_ASSERT_ERROR(condition, error)                                                                \
        {                                                                                                   \
            if (!(condition))                                                                               \
            {                                                                                               \
                ::std::ofstream errorFile("Log/fuckj", ::std::ios_base::out | ::std::ios_base::app);      \
                                                                                                            \
                errorFile << "Assertion failed at " << __FILE__ << ":" << __LINE__ << ::std::endl;          \
                errorFile << "Inside " << __FUNCTION__ << ::std::endl;                                      \
                errorFile << "Date: " << __DATE__ << ::std::endl;                                           \
                errorFile << "Time: " << __TIME__ << ::std::endl;                                           \
                errorFile << "Error: " << error << ::std::endl;                                             \
                errorFile << "Condition: " << (condition) << ::std::endl;                                   \
                errorFile << "############################################: " << ::std::endl;               \
            }                                                                                               \
        }                                                                                   
#   endif

#   ifndef TE_ASSERT_ERROR_SHORT
#   define TE_ASSERT_ERROR_SHORT(condition)                                                                 \
        {                                                                                                   \
            if (!(condition))                                                                               \
            {                                                                                               \
                ::std::ofstream errorFile(TE_ERROR_FILE, ::std::ios_base::out | ::std::ios_base::app);      \
                                                                                                            \
                errorFile << "Assertion failed at " << __FILE__ << ":" << __LINE__;                         \
                errorFile << " inside " << __FUNCTION__ << ::std::endl;                                     \
                errorFile << "Date: " << __DATE__ << ::std::endl;                                           \
                errorFile << "Time: " << __TIME__ << ::std::endl;                                           \
                errorFile << "Condition: " << (condition);                                                  \
                errorFile << "############################################: " << ::std::endl;               \
                assert((condition));                                                                        \
            }                                                                                               \
        }                                                                                   
#   endif
#else 
#   ifndef TE_ASSERT_ERROR
#       define TE_ASSERT_ERROR(condition, error) {}
#   endif

#   ifndef TE_ASSERT_ERROR_SHORT
#       define TE_ASSERT_ERROR_SHORT(condition) {}
#   endif
#endif
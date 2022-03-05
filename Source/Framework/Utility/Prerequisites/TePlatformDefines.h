#pragma once

// Initial platform/compiler-related stuff to set.
#define TE_PLATFORM_WIN32 1
#define TE_PLATFORM_LINUX 2

#define TE_COMPILER_MSVC 1
#define TE_COMPILER_GNUC 2
#define TE_COMPILER_INTEL 3
#define TE_COMPILER_CLANG 4

#define TE_ARCHITECTURE_x86_32 1
#define TE_ARCHITECTURE_x86_64 2

/* 
 * Currently it's hard to know before runtime if system is big endian or little endian
 * Here we are making the assumption that microprocessor are little endian
 */
#define TE_ENDIAN_LITTLE 1
#define TE_ENDIAN_BIG 2
#define TE_ENDIAN TE_ENDIAN_LITTLE 

#define TE_DEBUG_ENABLED 1
#define TE_DEBUG_DISABLED 0

// Finds the compiler type and version.
#if defined(__clang__)
#   define TE_COMPILER TE_COMPILER_CLANG
#   define TE_COMP_VER __clang_version__
#   define TE_THREADLOCAL __thread
#   define TE_STDCALL __attribute__((stdcall))
#   define TE_CDECL __attribute__((cdecl))
#   define TE_FALLTHROUGH [[clang::fallthrough]];
#elif defined(__GNUC__) // Check after Clang, as Clang defines this too
#   define TE_COMPILER TE_COMPILER_GNUC
#   define TE_COMP_VER (((__GNUC__)*100) + \
        (__GNUC_MINOR__*10) + \
        __GNUC_PATCHLEVEL__)
#   define TE_THREADLOCAL __thread
#   define TE_STDCALL __attribute__((stdcall))
#   define TE_CDECL __attribute__((cdecl))
#   define TE_FALLTHROUGH __attribute__((fallthrough));
#elif defined (__INTEL_COMPILER)
#   define TE_COMPILER TE_COMPILER_INTEL
#   define TE_COMP_VER __INTEL_COMPILER
#   define TE_STDCALL __stdcall
#   define TE_CDECL __cdecl
#   define TE_FALLTHROUGH
    // TE_THREADLOCAL define is down below because Intel compiler defines it differently based on platform
#elif defined(_MSC_VER) // Check after Clang and Intel, since we could be building with either within VS
#   define TE_COMPILER TE_COMPILER_MSVC
#   define TE_COMP_VER _MSC_VER
#   define TE_THREADLOCAL __declspec(thread)
#   define TE_STDCALL __stdcall
#   define TE_CDECL __cdecl
#   define TE_FALLTHROUGH
#   undef __PRETTY_FUNCTION__
#   define __PRETTY_FUNCTION__ __FUNCSIG__
#else
#   pragma error "No known compiler. "
#endif

// Finds the current platform
#if defined( __WIN32__ ) || defined( _WIN32 ) || defined(__CYGWIN__) || defined( __MINGW32__ )
#   define TE_PLATFORM TE_PLATFORM_WIN32
#else
#   define TE_PLATFORM TE_PLATFORM_LINUX
#endif

// Find the architecture type
#if defined(__x86_64__) || defined(_M_X64)
#   define TE_ARCH_TYPE TE_ARCHITECTURE_x86_64
#else
#   define TE_ARCH_TYPE TE_ARCHITECTURE_x86_32
#endif

// DLL export
#if TE_PLATFORM == TE_PLATFORM_WIN32 // Windows
#   if TE_COMPILER == TE_COMPILER_MSVC
#       if defined(TE_STATIC_LIB)
#           define TE_UTILITY_EXPORT
#       else
#           if defined(TE_UTILITY_EXPORTS)
#               define TE_UTILITY_EXPORT __declspec(dllexport)
#           else
#               define TE_UTILITY_EXPORT __declspec(dllimport)
#           endif
#       endif
#   else
#       if defined(TE_STATIC_LIB)
#           define TE_UTILITY_EXPORT
#       else
#           if defined(TE_UTILITY_EXPORTS)
#               define TE_UTILITY_EXPORT __attribute__ ((dllexport))
#           else
#               define TE_UTILITY_EXPORT __attribute__ ((dllimport))
#           endif
#       endif
#   endif
#
#
#define TE_UTILITY_HIDDEN
#else // Linux/Mac settings
#   define TE_UTILITY_EXPORT __attribute__ ((visibility ("default")))
#   define TE_UTILITY_HIDDEN __attribute__ ((visibility ("hidden")))
#endif

// DLL export for plugins
#if TE_PLATFORM == TE_PLATFORM_WIN32 // Windows
#   if TE_COMPILER == TE_COMPILER_MSVC
#       define TE_PLUGIN_EXPORT __declspec(dllexport)
#   else
#       define TE_PLUGIN_EXPORT __attribute__ ((dllexport))
#   endif
#   define TE_UTILITY_HIDDEN
#else // Linux/Mac settings
#   define TE_PLUGIN_EXPORT __attribute__ ((visibility ("default")))
#endif

// Windows Settings
#if TE_PLATFORM == TE_PLATFORM_WIN32
// Win32 compilers use _DEBUG for specifying debug builds.
// for MinGW, we set DEBUG
#   if defined(_DEBUG) || defined(DEBUG)
#       define TE_DEBUG_MODE TE_DEBUG_ENABLED
#   else
#       define TE_DEBUG_MODE TE_DEBUG_DISABLED
#   endif

#   if TE_COMPILER == TE_COMPILER_INTEL
#       define TE_THREADLOCAL __declspec(thread)
#   endif
#endif

// Linux/Mac Settings
#if TE_PLATFORM == TE_PLATFORM_LINUX
#   ifdef DEBUG
#       define TE_DEBUG_MODE TE_DEBUG_ENABLED
#   else
#   define TE_DEBUG_MODE TE_DEBUG_DISABLED
#   endif

#   if TE_COMPILER == TE_COMPILER_INTEL
#       define TE_THREADLOCAL __thread
#   endif
#endif

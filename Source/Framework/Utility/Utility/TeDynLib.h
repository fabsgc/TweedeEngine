#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

namespace te
{
#if TE_PLATFORM == TE_PLATFORM_WIN32
#    define DYNLIB_HANDLE hInstance
#    define DYNLIB_LOAD( a ) LoadLibraryEx( a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH )
#    define DYNLIB_GETSYM( a, b ) GetProcAddress( a, b )
#    define DYNLIB_UNLOAD( a ) !FreeLibrary( a )
#elif TE_PLATFORM == TE_PLATFORM_LINUX
#    define DYNLIB_HANDLE void*
#    define DYNLIB_LOAD( a ) dlopen( a, RTLD_LAZY | RTLD_GLOBAL)
#    define DYNLIB_GETSYM( a, b ) dlsym( a, b )
#    define DYNLIB_UNLOAD( a ) dlclose( a )
#endif

	class TE_UTILITY_EXPORT DynLib
	{
	public:
		DynLib(const String& name);
		~DynLib() = default;

		void  Load();
		void  Unload();
		const String& GetName() const { return _name; }
		void* GetSymbol(const String& strName) const;

	public:
#if TE_PLATFORM == TE_PLATFORM_LINUX
	static constexpr const char* EXTENSION = "so";
#elif TE_PLATFORM == TE_PLATFORM_WIN32
	static constexpr const char* EXTENSION = "dll";
#else
#   error Unhandled platform
#endif

#if TE_PLATFORM == TE_PLATFORM_LINUX
	static constexpr const char* PREFIX = "lib";
#elif TE_PLATFORM == TE_PLATFORM_WIN32
#   if defined(_MSC_VER)
		static constexpr const char* PREFIX = nullptr;
#   else
		static constexpr const char* PREFIX = "lib";
#   endif
#else
#   error Unhandled platform
#endif

	protected:
		friend class DynLibManager;

		String DynLibError();

	protected:
		String        _name;
		DYNLIB_HANDLE _handle = nullptr;
	};
}
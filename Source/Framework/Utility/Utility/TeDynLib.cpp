#include "Utility/TeDynLib.h"

#if TE_PLATFORM == TE_PLATFORM_WIN32
#   include <Windows.h>
#endif

namespace te
{
    DynLib::DynLib(const String& name)
        : _name(name)
    {
        Load();
    }

    void DynLib::Load()
    {
        if (_handle)
            return;

        _handle = (DYNLIB_HANDLE)DYNLIB_LOAD(_name.c_str());

        if (!_handle)
        {
            String error = DynLibError();
            TE_ASSERT_ERROR(false,
                "Could not load dynamic library " + _name +
                ".  System Error: " + error);
        }
        else
        {
            TE_DEBUG("Dynamic library " + _name + " has been successfully loaded");
        }
    }

    void DynLib::Unload()
    {
        if (!_handle)
            return;

        if (DYNLIB_UNLOAD(_handle))
        {
            TE_ASSERT_ERROR(false,
                "Could not unload dynamic library " + _name +
                ".  System Error: " + DynLibError());
        }

        TE_DEBUG("Dynamic library " + _name + " has been successfully unloaded");
    }

    void* DynLib::GetSymbol(const String& strName) const
    {
        if (!_handle)
            return nullptr;

        return (void*)DYNLIB_GETSYM(_handle, strName.c_str());
    }

    String DynLib::DynLibError()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        LPVOID lpMsgBuf;
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0,
            NULL
        );

        String ret((char*)lpMsgBuf);

        // Free the buffer.
        LocalFree(lpMsgBuf);
        return ret;
#else
        char *errstr;
        errstr = dlerror();
        TE_PRINT(String(errstr));
        return String();
#endif
    }
}

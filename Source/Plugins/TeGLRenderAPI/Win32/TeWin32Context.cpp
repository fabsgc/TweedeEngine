/* MD5#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif*/

#include "Win32/TeWin32Context.h"

namespace te
{
    Win32Context::Win32Context(HDC hdc, HGLRC glrc, bool ownsContext)
        : _HDC(hdc)
        , _Glrc(glrc)
        , _ownsContext(ownsContext)
    { }

    Win32Context::~Win32Context()
    {
        if (_ownsContext)
            ReleaseContext();
    }

    void Win32Context::SetCurrent(const RenderWindow& window)
    {
        if(wglMakeCurrent(_HDC, _Glrc) != TRUE)
            TE_ASSERT_ERROR(false, "wglMakeCurrent failed: " + TranslateWGLError());
    }

    void Win32Context::EndCurrent()
    {
        if(wglMakeCurrent(_HDC, 0) != TRUE)
            TE_ASSERT_ERROR(FACILITY_SETUPAPI, "wglMakeCurrent failed: " + TranslateWGLError());
    }

    void Win32Context::ReleaseContext()
    {
        if (_Glrc != 0)
        {
            if(wglDeleteContext(_Glrc) != TRUE)
                TE_ASSERT_ERROR(false, "wglDeleteContext failed: " + TranslateWGLError());

            _Glrc = 0;
            _HDC = 0;
        }
    }
}

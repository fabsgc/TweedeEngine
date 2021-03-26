#pragma once

#include "Win32/TeWin32Prerequisites.h"
#include "TeGLContext.h"

namespace te
{
    /**	Windows specific implementation of an OpenGL context. */
    class Win32Context : public GLContext
    {
    public:
        /**
         * Constructs a new context from a Windows device context and OpenGL rendering context. Optionally you may specify
         * that the context isn't owned by us (might be created by some external library), in which case it will not be
         * automatically released.
         */
        Win32Context(HDC hdc, HGLRC glrc, bool ownsContext);
        virtual ~Win32Context();

        /** @copydoc GLContext::SetCurrent */
        void SetCurrent(const RenderWindow& window) override;

        /** @copydoc GLContext::EndCurrent */
        void EndCurrent() override;

        /** @copydoc GLContext::ReleaseContext  */
        void ReleaseContext() override;

    protected:
        HDC _HDC;
        HGLRC _Glrc;
        bool _ownsContext;
    };
}

#pragma once

#include "TeGLContext.h"
#include <GL/glxew.h>

namespace te
{
    /**	Linux specific implementation of an OpenGL context. */
    class LinuxContext : public GLContext
    {
    public:
        /**
         * Constructs a new OpenGL context.
         *
         * @param[in] 	x11display 		X11 display all windows using this context will be created with.
         * @param[in]	visualInfo		X11 visual info describing properties of all windows that will use this context.
         **/
        LinuxContext(::Display* x11display, XVisualInfo& visualInfo);
        virtual ~LinuxContext();

        /** @copydoc GLContext::SetCurrent */
        void SetCurrent(const RenderWindow& window) override;

        /** @copydoc GLContext::EndCurrent */
        void EndCurrent() override;

        /** @copydoc GLContext::ReleaseContext  */
        void ReleaseContext() override;

    protected:
        ::Display* _display;
        ::Window _currentWindow = 0;
        GLXContext _context = 0;
    };
}

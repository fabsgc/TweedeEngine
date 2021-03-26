#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderWindow.h"

namespace te
{
    /**
     * Abstract class that encapsulated an OpenGL context. Each platform should provide its own GLContext specialization.
     */
    class GLContext
    {
    public:
        GLContext() = default;
        virtual ~GLContext() = default;

        /**	Activates the rendering context (all subsequent rendering commands will be executed on it). */
        virtual void SetCurrent(const RenderWindow& window) = 0;

        /**	Deactivates the rendering context. Normally called just before setCurrent is called on another context. */
        virtual void EndCurrent() = 0;

        /**	Releases the render context, freeing all of its resources. */
        virtual void ReleaseContext() {}
    };
}

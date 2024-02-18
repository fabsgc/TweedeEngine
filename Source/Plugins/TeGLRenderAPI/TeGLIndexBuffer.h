#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeIndexBuffer.h"
#include "TeGLHardwareBuffer.h"

namespace te
{
    /** OpenGL implementation of an index buffer. */
    class GLIndexBuffer : public IndexBuffer
    {
    public:
        GLIndexBuffer(const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask);

        /**	Returns internal OpenGL index buffer handle. */
        GLuint GetGLBufferId() const { return static_cast<GLHardwareBuffer*>(_buffer)->GetGLBufferId(); }

    protected:
        /** @copydoc IndexBuffer::Initialize */
        void Initialize() override;
    };
}

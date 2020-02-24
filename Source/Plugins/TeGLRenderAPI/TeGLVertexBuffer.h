#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeVertexBuffer.h"
#include "TeGLHardwareBuffer.h"

namespace te
{
    /** OpenGL implementation of a vertex buffer. */
    class GLVertexBuffer : public VertexBuffer
    {
    public:
        GLVertexBuffer(const VERTEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask);

    protected:
        /** @copydoc VertexBuffer::Initialize */
        void Initialize() override;

        bool _streamOut;
    };

    // TODO
}

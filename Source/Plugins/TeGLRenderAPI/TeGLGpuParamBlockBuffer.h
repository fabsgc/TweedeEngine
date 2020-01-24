#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuParamBlockBuffer.h"

namespace te
{
    /** OpenGL implementation of a parameter block buffer (constant buffer). */
    class GLGpuParamBlockBuffer : public GpuParamBlockBuffer
    {
    public:
        GLGpuParamBlockBuffer(UINT32 size, GpuBufferUsage usage, GpuDeviceFlags deviceMask);
        ~GLGpuParamBlockBuffer();

    protected:
        /** @copydoc GpuParamBlockBuffer::initialize */
        void Initialize() override;
    };
}

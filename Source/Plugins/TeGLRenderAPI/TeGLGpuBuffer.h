#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuBuffer.h"

namespace te
{
    /** OpenGL implementation of a generic GPU buffer. */
    class GLGpuBuffer : public GpuBuffer
    {
    public:
        ~GLGpuBuffer();

    protected:
        friend class GLHardwareBufferManager;

        GLGpuBuffer(const GPU_BUFFER_DESC& desc, GpuDeviceFlags deviceMask);
        GLGpuBuffer(const GPU_BUFFER_DESC& desc, SPtr<HardwareBuffer> underlyingBuffer);

        /** @copydoc GpuBuffer::Initialize */
        void Initialize() override;

        // TODO
    };
}

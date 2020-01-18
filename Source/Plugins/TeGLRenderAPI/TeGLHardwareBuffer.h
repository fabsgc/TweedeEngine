#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeHardwareBuffer.h"

namespace te
{
    /**	Class containing common functionality for all OpenGL hardware buffers. */
    class GLHardwareBuffer : public HardwareBuffer
    {
    public:
        GLHardwareBuffer(GpuBufferUsage usage, UINT32 elementCount, UINT32 elementSize,
            bool systemMemory = false, bool streamOut = false);
        ~GLHardwareBuffer();

        /** @copydoc HardwareBuffer::ReadData */
        void ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

        /** @copydoc HardwareBuffer::WriteData */
        void WriteData(UINT32 offset, UINT32 length, const void* source,
            BufferWriteType writeFlags = BWT_NORMAL, UINT32 queueIdx = 0) override;

        /** @copydoc HardwareBuffer::CopyData */
        void CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset,
            UINT32 length, bool discardWholeBuffer = false) override;

    protected:
        /** @copydoc HardwareBuffer::Map */
        void* Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx) override;

        /** @copydoc HardwareBuffer::Unmap */
        void Unmap() override;
    };
}

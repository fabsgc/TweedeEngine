#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeHardwareBuffer.h"
#include "Utility/TePoolAllocator.h"

namespace te
{
    /** Class containing common functionality for all OpenGL hardware buffers. */
    class GLHardwareBuffer : public HardwareBuffer
    {
    public:
        /** Creates and initializes the buffer object. */
        GLHardwareBuffer(GLenum target, UINT32 size, GpuBufferUsage usage);
        ~GLHardwareBuffer();

        /** @copydoc HardwareBuffer::ReadData */
        void ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

        /** @copydoc HardwareBuffer::WriteData */
        void WriteData(UINT32 offset, UINT32 length, const void* source,
            BufferWriteType writeFlags = BWT_NORMAL, UINT32 queueIdx = 0) override;

        /** @copydoc HardwareBuffer::CopyData */
        void CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset,
            UINT32 length, bool discardWholeBuffer = false) override;

        /**	Returns internal OpenGL buffer ID. */
        GLuint getGLBufferId() const { return _bufferId; }

    protected:
        /** @copydoc HardwareBuffer::Map */
        void* Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx) override;

        /** @copydoc HardwareBuffer::Unmap */
        void Unmap() override;

    protected:
        GLenum _target;
        GLuint _bufferId = 0;
    };

    IMPLEMENT_GLOBAL_POOL(GLHardwareBuffer, 32)
}

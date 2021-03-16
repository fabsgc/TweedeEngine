#include "TeGLHardwareBuffer.h"

namespace te
{
    GLHardwareBuffer::GLHardwareBuffer(GLenum target, UINT32 size, GpuBufferUsage usage)
        : HardwareBuffer(size, usage, GDF_DEFAULT)
        , _target(target)
    { 
        // TODO
    }

    GLHardwareBuffer::~GLHardwareBuffer()
    { 
        if (_bufferId != 0)
        {
            glDeleteBuffers(1, &_bufferId);
            TE_CHECK_GL_ERROR();
        }
    }

    void* GLHardwareBuffer::Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx)
    {
        return nullptr;
    }

    void GLHardwareBuffer::Unmap()
    { 
        // TODO
    }

    void GLHardwareBuffer::CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset,
        UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
    {
        // TODO
    }

    void GLHardwareBuffer::ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx, UINT32 queueIdx)
    { 
        // TODO
    }

    void GLHardwareBuffer::WriteData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags,
        UINT32 queueIdx)
    { 
        // TODO
    }
}

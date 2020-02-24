#include "TeGLHardwareBuffer.h"

namespace te
{
    GLHardwareBuffer::GLHardwareBuffer(GpuBufferUsage usage, UINT32 elementCount, UINT32 elementSize, bool useSystemMem, bool streamOut)
        : HardwareBuffer(elementCount* elementSize, usage, GDF_DEFAULT)
    { }

    GLHardwareBuffer::~GLHardwareBuffer()
    { }

    void* GLHardwareBuffer::Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx)
    {
        return nullptr;
    }

    void GLHardwareBuffer::Unmap()
    { }

    void GLHardwareBuffer::CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset,
        UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
    { }

    void GLHardwareBuffer::ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx, UINT32 queueIdx)
    { }

    void GLHardwareBuffer::WriteData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags,
        UINT32 queueIdx)
    { }
}

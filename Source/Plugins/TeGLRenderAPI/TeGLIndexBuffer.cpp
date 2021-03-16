#include "TeGLIndexBuffer.h"

namespace te
{
    static void DeleteBuffer(HardwareBuffer* buffer)
    {
        te_delete(static_cast<GLHardwareBuffer*>(buffer));
    }

    GLIndexBuffer::GLIndexBuffer(const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
        : IndexBuffer(desc, deviceMask)
    {
        assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on DirectX.");
    }

    void GLIndexBuffer::Initialize()
    {
        _buffer = te_new<GLHardwareBuffer>(GL_ELEMENT_ARRAY_BUFFER, _size, _usage);
        _bufferDeleter = &DeleteBuffer;

        IndexBuffer::Initialize();
    }
}

#include "TeGLVertexBuffer.h"

namespace te
{
    static void DeleteBuffer(HardwareBuffer* buffer)
    {
        te_delete(static_cast<GLHardwareBuffer*>(buffer));
    }

    GLVertexBuffer::GLVertexBuffer(const VERTEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
        : VertexBuffer(desc, deviceMask)
        , _streamOut(desc.StreamOut)
    {
        assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on DirectX 11.");
    }

    void GLVertexBuffer::Initialize()
    {
        _buffer = te_new<GLHardwareBuffer>(_usage, 1, _size,false, _streamOut);
        _bufferDeleter = &DeleteBuffer;

        VertexBuffer::Initialize();
    }
}

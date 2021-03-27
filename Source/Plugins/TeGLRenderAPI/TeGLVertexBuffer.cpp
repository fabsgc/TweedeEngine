#include "TeGLVertexBuffer.h"

namespace te
{
    static void DeleteBuffer(HardwareBuffer* buffer)
    {
        te_delete(static_cast<GLHardwareBuffer*>(buffer));
    }

    GLVertexBuffer::~GLVertexBuffer()
    {
        while (!_VAObjects.empty())
            GLVertexArrayObjectManager::Instance().NotifyBufferDestroyed(_VAObjects[0]);
    }

    GLVertexBuffer::GLVertexBuffer(const VERTEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
        : VertexBuffer(desc, deviceMask)
    {
        assert((deviceMask == GDF_DEFAULT || deviceMask == GDF_PRIMARY) && "Multiple GPUs not supported natively on DirectX 11.");
    }

    void GLVertexBuffer::Initialize()
    {
        _buffer = te_new<GLHardwareBuffer>(GL_ARRAY_BUFFER, _size, _usage);
        _bufferDeleter = &DeleteBuffer;

        VertexBuffer::Initialize();
    }

    void GLVertexBuffer::RegisterVAO(const GLVertexArrayObject& vao)
    {
        _VAObjects.push_back(vao);
    }

    void GLVertexBuffer::UnregisterVAO(const GLVertexArrayObject& vao)
    {
        const auto iterFind = std::find(_VAObjects.begin(), _VAObjects.end(), vao);

        if (iterFind != _VAObjects.end())
            _VAObjects.erase(iterFind);
    }
}

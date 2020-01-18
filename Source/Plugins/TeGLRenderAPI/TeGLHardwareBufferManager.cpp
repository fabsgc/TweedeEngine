#include "TeGLHardwareBufferManager.h"
#include "TeGLVertexBuffer.h"
#include "TeGLIndexBuffer.h"

namespace te
{
    GLHardwareBufferManager::GLHardwareBufferManager()
    { }

    SPtr<VertexBuffer> GLHardwareBufferManager::CreateVertexBufferInternal(const VERTEX_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        SPtr<GLVertexBuffer> ret = te_shared_ptr_new<GLVertexBuffer>(desc, deviceMask);
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<IndexBuffer> GLHardwareBufferManager::CreateIndexBufferInternal(const INDEX_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        SPtr<GLIndexBuffer> ret = te_shared_ptr_new<GLIndexBuffer>(desc, deviceMask);
        ret->SetThisPtr(ret);

        return ret;
    }
}

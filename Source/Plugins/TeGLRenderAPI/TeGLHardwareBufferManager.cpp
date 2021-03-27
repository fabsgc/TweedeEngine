#include "TeGLHardwareBufferManager.h"
#include "TeGLVertexBuffer.h"
#include "TeGLIndexBuffer.h"
#include "TeGLGpuParamBlockBuffer.h"
#include "TeGLGpuBuffer.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(GLHardwareBufferManager)

    GLHardwareBufferManager::GLHardwareBufferManager()
    { }

    SPtr<VertexBuffer> GLHardwareBufferManager::CreateVertexBufferInternal(const VERTEX_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        SPtr<GLVertexBuffer> ret = te_core_ptr_new<GLVertexBuffer>(desc, deviceMask);
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<IndexBuffer> GLHardwareBufferManager::CreateIndexBufferInternal(const INDEX_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        SPtr<GLIndexBuffer> ret = te_core_ptr_new<GLIndexBuffer>(desc, deviceMask);
        ret->SetThisPtr(ret);

        return ret;
    }

    SPtr<GpuParamBlockBuffer> GLHardwareBufferManager::CreateGpuParamBlockBufferInternal(UINT32 size,
        GpuBufferUsage usage, GpuDeviceFlags deviceMask)
    {
        GLGpuParamBlockBuffer* paramBlockBuffer =
            new (te_allocate<GLGpuParamBlockBuffer>()) GLGpuParamBlockBuffer(size, usage, deviceMask);

        SPtr<GpuParamBlockBuffer> paramBlockBufferPtr = te_core_ptr<GLGpuParamBlockBuffer>(paramBlockBuffer);
        paramBlockBufferPtr->SetThisPtr(paramBlockBufferPtr);

        return paramBlockBufferPtr;
    }

    SPtr<GpuBuffer> GLHardwareBufferManager::CreateGpuBufferInternal(const GPU_BUFFER_DESC& desc,
        GpuDeviceFlags deviceMask)
    {
        GLGpuBuffer* buffer = new (te_allocate<GLGpuBuffer>()) GLGpuBuffer(desc, deviceMask);

        SPtr<GLGpuBuffer> bufferPtr = te_core_ptr<GLGpuBuffer>(buffer);
        bufferPtr->SetThisPtr(bufferPtr);

        return bufferPtr;
    }

    SPtr<GpuBuffer> GLHardwareBufferManager::CreateGpuBufferInternal(const GPU_BUFFER_DESC& desc,
        SPtr<HardwareBuffer> underlyingBuffer)
    {
        GLGpuBuffer* buffer = new (te_allocate<GLGpuBuffer>()) GLGpuBuffer(desc, std::move(underlyingBuffer));

        SPtr<GLGpuBuffer> bufferPtr = te_core_ptr<GLGpuBuffer>(buffer);
        bufferPtr->SetThisPtr(bufferPtr);

        return bufferPtr;
    }

    GLenum GLHardwareBufferManager::GetGLUsage(GpuBufferUsage usage)
    {
        if((usage & GBU_LOADSTORE) == GBU_LOADSTORE)
        {
            if ((usage & GBU_STATIC) != 0)
                return GL_STATIC_READ;

            return GL_DYNAMIC_READ;
        }
        else
        {
            if ((usage & GBU_STATIC) != 0)
                return GL_STATIC_DRAW;

            return GL_DYNAMIC_DRAW;
        }
    }

    GLenum GLHardwareBufferManager::GetGLType(VertexElementType type)
    {
        switch(type)
        {
            case VET_FLOAT1:
            case VET_FLOAT2:
            case VET_FLOAT3:
            case VET_FLOAT4:
                return GL_FLOAT;
            case VET_SHORT1:
            case VET_SHORT2:
            case VET_SHORT4:
                return GL_SHORT;
            case VET_USHORT1:
            case VET_USHORT2:
            case VET_USHORT4:
                return GL_UNSIGNED_SHORT;
            case VET_INT1:
            case VET_INT2:
            case VET_INT3:
            case VET_INT4:
                return GL_INT;
            case VET_UINT1:
            case VET_UINT2:
            case VET_UINT3:
            case VET_UINT4:
                return GL_UNSIGNED_INT;
            case VET_COLOR:
            case VET_COLOR_ABGR:
            case VET_COLOR_ARGB:
            case VET_UBYTE4:
            case VET_UBYTE4_NORM:
                return GL_UNSIGNED_BYTE;
            default:
                return 0;
        };
    }
}

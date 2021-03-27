#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeHardwareBufferManager.h"

namespace te
{
    /** Handles creation of OpenGL hardware buffers. */
    class GLHardwareBufferManager : public HardwareBufferManager
    {
    public:
        GLHardwareBufferManager();

        /**	Converts engine buffer usage flags into OpenGL specific flags. */
        static GLenum GetGLUsage(GpuBufferUsage usage);

        /**	Converts vertex element type into OpenGL specific type. */
        static GLenum GetGLType(VertexElementType type);

    protected:
        /** @copydoc HardwareBufferManager::CreateVertexBufferInternal */
        SPtr<VertexBuffer> CreateVertexBufferInternal(const VERTEX_BUFFER_DESC& desc,
            GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

        /** @copydoc HardwareBufferManager::createIndexBufferInternal */
        SPtr<IndexBuffer> CreateIndexBufferInternal(const INDEX_BUFFER_DESC& desc,
            GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

        /** @copydoc HardwareBufferManager::CreateGpuParamBlockBufferInternal  */
        SPtr<GpuParamBlockBuffer> CreateGpuParamBlockBufferInternal(UINT32 size,
            GpuBufferUsage usage = GBU_DYNAMIC, GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

            /** @copydoc HardwareBufferManager::createGpuBufferInternal(const GPU_BUFFER_DESC&, GpuDeviceFlags) */
        SPtr<GpuBuffer> CreateGpuBufferInternal(const GPU_BUFFER_DESC& desc,
            GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

        /** @copydoc HardwareBufferManager::createGpuBufferInternal(const GPU_BUFFER_DESC&, SPtr<HardwareBuffer>) */
        SPtr<GpuBuffer> CreateGpuBufferInternal(const GPU_BUFFER_DESC& desc,
            SPtr<HardwareBuffer> underlyingBuffer) override;
    };
}

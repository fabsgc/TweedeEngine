#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeHardwareBufferManager.h"

namespace te
{
    /**	Handles creation of OpenGL hardware buffers. */
    class GLHardwareBufferManager : public HardwareBufferManager
    {
    public:
        GLHardwareBufferManager();

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
    };
}

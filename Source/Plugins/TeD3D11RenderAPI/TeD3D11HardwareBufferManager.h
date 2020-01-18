#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeHardwareBufferManager.h"

namespace te
{
    /**	Handles creation of DirectX 11 hardware buffers. */
    class D3D11HardwareBufferManager : public HardwareBufferManager
    {
    public:
        D3D11HardwareBufferManager(D3D11Device& device);

    protected:
        /** @copydoc HardwareBufferManager::CreateVertexBufferInternal */
        SPtr<VertexBuffer> CreateVertexBufferInternal(const VERTEX_BUFFER_DESC& desc,
            GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

        /** @copydoc HardwareBufferManager::createIndexBufferInternal */
        SPtr<IndexBuffer> CreateIndexBufferInternal(const INDEX_BUFFER_DESC& desc,
            GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

    protected:
        D3D11Device& _device;
    };
}

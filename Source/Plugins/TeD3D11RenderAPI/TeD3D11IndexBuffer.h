#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeIndexBuffer.h"
#include "TeD3D11HardwareBuffer.h"

namespace te 
{
    /** DirectX 11 implementation of an index buffer. */
    class D3D11IndexBuffer : public IndexBuffer
    {
    public:
        D3D11IndexBuffer(D3D11Device& device, const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask);

        /**	Gets the internal DX11 index buffer object. */
        ID3D11Buffer* GetD3DIndexBuffer() const { return static_cast<D3D11HardwareBuffer*>(_buffer)->GetD3DBuffer(); }

    protected:
        /** @copydoc IndexBuffer::Initialize */
        void Initialize() override;

        D3D11Device& _device;
    };
}

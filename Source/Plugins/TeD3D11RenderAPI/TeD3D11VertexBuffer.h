#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeVertexBuffer.h"
#include "TeD3D11HardwareBuffer.h"

namespace te 
{
    /** DirectX 11 implementation of a vertex buffer. */
    class D3D11VertexBuffer : public VertexBuffer
    {
    public:
        D3D11VertexBuffer(D3D11Device& device, const VERTEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask);

        /** Get the D3D-specific index buffer */
        ID3D11Buffer* GetD3DVertexBuffer() const { return static_cast<D3D11HardwareBuffer*>(_buffer)->GetD3DBuffer(); }

    protected:
        /** @copydoc VertexBuffer::Initialize */
        void Initialize() override;

        D3D11Device& _device;
        bool _streamOut;
    };
}

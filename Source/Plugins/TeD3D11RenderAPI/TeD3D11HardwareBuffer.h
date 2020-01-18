#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeHardwareBuffer.h"

namespace te
{
    /**	Class containing common functionality for all DirectX 11 hardware buffers. */
    class D3D11HardwareBuffer : public HardwareBuffer
    {
    public:
        /**	Available types of DX11 buffers. */
        enum BufferType
        {
            /** Contains geometry vertices and their properties. */
            BT_VERTEX = 1 << 0,
            /** Contains triangle to vertex mapping. */
            BT_INDEX = 1 << 1,
            /** Contains GPU program parameters. */
            BT_CONSTANT = 1 << 2,
            /** Generic buffer that contains primitive types. */
            BT_STANDARD = 1 << 3,
            /** Generic buffer that holds one or more user-defined structures laid out sequentially. */
            BT_STRUCTURED = 1 << 4,
            /** Generic buffer that holds raw block of bytes with no defined structure. */
            BT_RAW = 1 << 5,
            /** Generic buffer that is used for holding parameters used for indirect rendering. */
            BT_INDIRECTARGUMENT = 1 << 6,
        };

        D3D11HardwareBuffer(BufferType btype, GpuBufferUsage usage, UINT32 elementCount, UINT32 elementSize,
            D3D11Device& device, bool systemMemory = false, bool streamOut = false);
        ~D3D11HardwareBuffer();

        /** @copydoc HardwareBuffer::ReadData */
        void ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

        /** @copydoc HardwareBuffer::WriteData */
        void WriteData(UINT32 offset, UINT32 length, const void* source,
            BufferWriteType writeFlags = BWT_NORMAL, UINT32 queueIdx = 0) override;

        /** @copydoc HardwareBuffer::CopyData */
        void CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset,
            UINT32 length, bool discardWholeBuffer = false) override;

        /**	Returns the internal DX11 buffer object. */
        ID3D11Buffer* GetD3DBuffer() const { return _D3DBuffer; }

    protected:
        /** @copydoc HardwareBuffer::Map */
        void* Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx) override;

        /** @copydoc HardwareBuffer::Unmap */
        void Unmap() override;

        BufferType _bufferType;
        UINT32 _elementCount;
        UINT32 _elementSize;
        GpuBufferUsage _usage;

        ID3D11Buffer* _D3DBuffer = nullptr;

        bool _useTempStagingBuffer = false;
        D3D11HardwareBuffer* _pTempStagingBuffer = nullptr;
        bool _stagingUploadNeeded;

        D3D11Device& _device;
        D3D11_BUFFER_DESC _desc;
    };
}

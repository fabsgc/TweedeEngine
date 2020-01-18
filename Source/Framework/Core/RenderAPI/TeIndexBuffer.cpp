#include "TeIndexBuffer.h"
#include "RenderAPI/TeHardwareBufferManager.h"
#include "RenderAPI/TeRenderAPI.h"

namespace te
{
    UINT32 CalcIndexSize(IndexType type)
    {
        switch (type)
        {
        case IT_16BIT:
            return sizeof(unsigned short);
        default:
        case IT_32BIT:
            return sizeof(unsigned int);
        }
    }

    void CheckValidDesc(const INDEX_BUFFER_DESC& desc)
    {
        if (desc.NumIndices == 0)
        {
            TE_ASSERT_ERROR(false, "Index buffer index count is not allowed to be zero.", __FILE__, __LINE__);
        }
    }

    IndexBufferProperties::IndexBufferProperties(IndexType idxType, UINT32 numIndices)
        : _indexType(idxType)
        , _numIndices(numIndices)
        , _indexSize(CalcIndexSize(idxType))
    { }

    IndexBuffer::IndexBuffer(const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
        : HardwareBuffer(CalcIndexSize(desc.Type)* desc.NumIndices, desc.Usage, deviceMask)
        , _properties(desc.Type, desc.NumIndices)
    {
#if TE_DEBUG_MODE
        CheckValidDesc(desc);
#endif
    }

    IndexBuffer::~IndexBuffer()
    {
        if (_buffer && !_sharedBuffer)
        {
            _bufferDeleter(_buffer);
        }
    }

    void IndexBuffer::Initialize()
    {
        CoreObject::Initialize();
    }

    void* IndexBuffer::Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx)
    {
        return _buffer->Lock(offset, length, options, deviceIdx, queueIdx);
    }

    void IndexBuffer::Unmap()
    {
        _buffer->Unlock();
    }

    void IndexBuffer::ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx, UINT32 queueIdx)
    {
        _buffer->ReadData(offset, length, dest, deviceIdx, queueIdx);
    }

    void IndexBuffer::WriteData(UINT32 offset, UINT32 length, const void* source, BufferWriteType writeFlags,
        UINT32 queueIdx)
    {
        _buffer->WriteData(offset, length, source, writeFlags, queueIdx);
    }

    void IndexBuffer::CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset, UINT32 length,
        bool discardWholeBuffer)
    {
        auto& srcIndexBuffer = static_cast<IndexBuffer&>(srcBuffer);
        _buffer->CopyData(*srcIndexBuffer._buffer, srcOffset, dstOffset, length, discardWholeBuffer);
    }

    SPtr<IndexBuffer> IndexBuffer::Create(const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateIndexBuffer(desc, deviceMask);
    }
}

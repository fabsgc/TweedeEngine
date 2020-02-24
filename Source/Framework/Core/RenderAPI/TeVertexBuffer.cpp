#include "TeVertexBuffer.h"
#include "RenderAPI/TeHardwareBufferManager.h"

namespace te
{
    void CheckValidDesc(const VERTEX_BUFFER_DESC& desc)
    {
        if(desc.VertexSize == 0)
        {
            TE_ASSERT_ERROR(false, "Vertex buffer vertex size is not allowed to be zero.", __FILE__, __LINE__);
        }

        if(desc.NumVerts == 0)
        {
            TE_ASSERT_ERROR(false, "Vertex buffer vertex count is not allowed to be zero.", __FILE__, __LINE__);
        }
    }

    VertexBufferProperties::VertexBufferProperties(UINT32 numVertices, UINT32 vertexSize)
        : _numVertices(numVertices)
        , _vertexSize(vertexSize)
    { }

    VertexBuffer::VertexBuffer(const VERTEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
        : HardwareBuffer(desc.VertexSize * desc.NumVerts, desc.Usage, deviceMask)
        , _properties(desc.NumVerts, desc.VertexSize)
        , _streamOut(desc.StreamOut)
    {
#if TE_DEBUG_MODE
        CheckValidDesc(desc);
#endif
    }

    VertexBuffer::~VertexBuffer()
    {
        if(_buffer && !_sharedBuffer)
        {
            _bufferDeleter(_buffer);
        }
    }

    void VertexBuffer::Initialize()
    {
        CoreObject::Initialize();
    }

    void* VertexBuffer::Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx)
    {
        return _buffer->Lock(offset, length, options, deviceIdx, queueIdx);
    }

    void VertexBuffer::Unmap()
    {
        _buffer->Unlock();
    }

    void VertexBuffer::ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx, UINT32 queueIdx)
    {
        _buffer->ReadData(offset, length, dest, deviceIdx, queueIdx);
    }

    void VertexBuffer::WriteData(UINT32 offset, UINT32 length, const void* source, BufferWriteType writeFlags,
        UINT32 queueIdx)
    {
        _buffer->WriteData(offset, length, source, writeFlags, queueIdx);
    }

    void VertexBuffer::CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset,
        UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
    {
        auto& srcVertexBuffer = static_cast<VertexBuffer&>(srcBuffer);
        _buffer->CopyData(*srcVertexBuffer._buffer, srcOffset, dstOffset, length, discardWholeBuffer);
    }

    SPtr<VertexBuffer> VertexBuffer::Create(const VERTEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateVertexBuffer(desc, deviceMask);
    }
}

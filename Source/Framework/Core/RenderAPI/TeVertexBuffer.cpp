#include "TeVertexBuffer.h"
#include "RenderAPI/TeGpuParams.h"
#include "RenderAPI/TeHardwareBufferManager.h"
#include "Profiling/TeProfilerGPU.h"

namespace te
{
    void CheckValidDesc(const VERTEX_BUFFER_DESC& desc)
    {
        if(desc.VertexSize == 0)
        {
            TE_ASSERT_ERROR(false, "Vertex buffer vertex size is not allowed to be zero.");
        }

        if(desc.NumVerts == 0)
        {
            TE_ASSERT_ERROR(false, "Vertex buffer vertex count is not allowed to be zero.");
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
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        CheckValidDesc(desc);
#endif
    }

    VertexBuffer::~VertexBuffer()
    {
        if(_buffer && !_sharedBuffer)
            _bufferDeleter(_buffer);

        TE_INC_PROFILER_GPU(ResDestroyed);
    }

    void VertexBuffer::Initialize()
    {
        TE_INC_PROFILER_GPU(ResCreated);
        CoreObject::Initialize();
    }

    void* VertexBuffer::Map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx)
    {
#if TE_PROFILING_ENABLED
        if (options == GBL_READ_ONLY || options == GBL_READ_WRITE)
        {
            TE_INC_PROFILER_GPU(ResRead);
        }

        if (options == GBL_READ_WRITE || options == GBL_WRITE_ONLY || options == GBL_WRITE_ONLY_DISCARD || options == GBL_WRITE_ONLY_NO_OVERWRITE)
        {
            TE_INC_PROFILER_GPU(ResWrite);
        }
#endif
        return _buffer->Lock(offset, length, options, deviceIdx, queueIdx);
    }

    void VertexBuffer::Unmap()
    {
        _buffer->Unlock();
    }

    void VertexBuffer::ReadData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx, UINT32 queueIdx)
    {
        _buffer->ReadData(offset, length, dest, deviceIdx, queueIdx);
        TE_INC_PROFILER_GPU(ResRead);
    }

    void VertexBuffer::WriteData(UINT32 offset, UINT32 length, const void* source, BufferWriteType writeFlags,
        UINT32 queueIdx)
    {
        _buffer->WriteData(offset, length, source, writeFlags, queueIdx);
        TE_INC_PROFILER_GPU(ResWrite);
    }

    void VertexBuffer::CopyData(HardwareBuffer& srcBuffer, UINT32 srcOffset,
        UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
    {
        auto& srcVertexBuffer = static_cast<VertexBuffer&>(srcBuffer);
        _buffer->CopyData(*srcVertexBuffer._buffer, srcOffset, dstOffset, length, discardWholeBuffer);
    }

    SPtr<GpuBuffer> VertexBuffer::GetLoadStore(GpuBufferType type, GpuBufferFormat format, UINT32 elementSize)
    {
        if ((_usage & GBU_LOADSTORE) != GBU_LOADSTORE)
            return nullptr;

        for (const auto& entry : _loadStoreViews)
        {
            const GpuBufferProperties& props = entry->GetProperties();
            if (props.GetType() == type)
            {
                if (type == GBT_STANDARD && props.GetFormat() == format)
                    return entry;

                if (type == GBT_STRUCTURED && props.GetElementSize() == elementSize)
                    return entry;
            }
        }

        UINT32 elemSize = type == GBT_STANDARD ? te::GpuBuffer::GetFormatSize(format) : elementSize;
        if ((_buffer->GetSize() % elemSize) != 0)
        {
            TE_DEBUG("Size of the buffer isn't divisible by individual element size provided for the buffer view.");
            return nullptr;
        }

        GPU_BUFFER_DESC desc;
        desc.Type = type;
        desc.Format = format;
        desc.Usage = _usage;
        desc.ElementSize = elementSize;
        desc.ElementCount = _buffer->GetSize() / elemSize;

        if (!_sharedBuffer)
            _sharedBuffer = te_shared_ptr(_buffer, _bufferDeleter);

        SPtr<GpuBuffer> newView = GpuBuffer::Create(desc, _sharedBuffer);
        _loadStoreViews.push_back(newView);

        return newView;
    }

    SPtr<VertexBuffer> VertexBuffer::Create(const VERTEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateVertexBuffer(desc, deviceMask);
    }
}

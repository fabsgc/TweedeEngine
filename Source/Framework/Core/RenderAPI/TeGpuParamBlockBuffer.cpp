#include "RenderAPI/TeGpuParamBlockBuffer.h"
#include "RenderAPI/TeHardwareBuffer.h"
#include "RenderAPI/TeHardwareBufferManager.h"
#include "Profiling/TeProfilerGPU.h"

namespace te
{
    GpuParamBlockBuffer::GpuParamBlockBuffer(UINT32 size, GpuBufferUsage usage, GpuDeviceFlags deviceMask)
        : _buffer(nullptr)
        , _usage(usage)
        , _size(size)
        , _cachedData(nullptr)
        , _GPUBufferDirty(false)
        
    {
        if (_size > 0)
        {
            _cachedData = (UINT8*)te_allocate(_size);
            memset(_cachedData, 0, _size);
        }
    }

    GpuParamBlockBuffer::~GpuParamBlockBuffer()
    {
        TE_INC_PROFILER_GPU(ResDestroyed);

        if (_cachedData != nullptr)
            te_free(_cachedData);
    }

    void GpuParamBlockBuffer::Initialize()
    {
        TE_INC_PROFILER_GPU(ResCreated);

        CoreObject::Initialize();
    }

    void GpuParamBlockBuffer::Write(UINT32 offset, const void* data, UINT32 size)
    {
#if TE_DEBUG_MODE
        if ((offset + size) > _size)
        {
            TE_ASSERT_ERROR(false, "Wanted range is out of buffer bounds. Available range: 0 .. " + ToString(_size) + ". " +
                "Wanted range: " + ToString(offset) + " .. " + ToString(offset + size) + ".");
        }
#endif

        memcpy(_cachedData + offset, data, size);
        _GPUBufferDirty = true;
    }

    void GpuParamBlockBuffer::Read(UINT32 offset, void* data, UINT32 size)
    {
#if TE_DEBUG_MODE
        if ((offset + size) > _size)
        {
            TE_ASSERT_ERROR(false, "Wanted range is out of buffer bounds. Available range: 0 .. " + ToString(_size) + ". " +
                "Wanted range: " + ToString(offset) + " .. " + ToString(offset + size) + ".");
        }
#endif

        memcpy(data, _cachedData + offset, size);
        _GPUBufferDirty = true;
    }

    void GpuParamBlockBuffer::ZeroOut(UINT32 offset, UINT32 size)
    {
#if TE_DEBUG_MODE
        if ((offset + size) > _size)
        {
            TE_ASSERT_ERROR(false, "Wanted range is out of buffer bounds. Available range: 0 .. " + ToString(_size) + ". " +
                "Wanted range: " + ToString(offset) + " .. " + ToString(offset + size) + ".");
        }
#endif

        memset(_cachedData + offset, 0, size);
        _GPUBufferDirty = true;
    }

    void GpuParamBlockBuffer::FlushToGPU(UINT32 queueIdx)
    {
        if (_GPUBufferDirty)
        {
            WriteToGPU(_cachedData, queueIdx);
            _GPUBufferDirty = false;
        }
    }

    void GpuParamBlockBuffer::WriteToGPU(const UINT8* data, UINT32 queueIdx)
    {
        _buffer->WriteData(0, _size, data, BWT_DISCARD, queueIdx);
        TE_INC_PROFILER_GPU(ResWrite);
    }

    SPtr<GpuParamBlockBuffer> GpuParamBlockBuffer::Create(UINT32 size, GpuBufferUsage usage, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateGpuParamBlockBuffer(size, usage, deviceMask);
    }
}

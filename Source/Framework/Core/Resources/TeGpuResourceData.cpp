#include "Resources/TeGpuResourceData.h"

#include <thread>
#include <chrono>

namespace te
{
    GpuResourceData::GpuResourceData(const GpuResourceData& copy)
    {
        _data = copy._data;
        _ownsData = false;
    }

    GpuResourceData::~GpuResourceData()
    {
        FreeInternalBuffer();
    }

    GpuResourceData& GpuResourceData::operator=(const GpuResourceData& rhs)
    {
        _data = rhs._data;
        _ownsData = false;

        return *this;
    }

    UINT8* GpuResourceData::GetData() const
    {
        return _data;
    }

    void GpuResourceData::SetData(UPtr<UINT8[]> &data)
    {
        FreeInternalBuffer();

        _data = data.release();
        _ownsData = true;
    }

    void GpuResourceData::AllocateInternalBuffer()
    {
        AllocateInternalBuffer(GetInternalBufferSize());
    }

    void GpuResourceData::AllocateInternalBuffer(UINT32 size)
    {
        UINT32 maxTry = 3;
        std::atomic<UINT32> tryCount = 0;
        FreeInternalBuffer();

        do
        {
            // In case of intensive buffer creation, this call could fail
            // So I put te_allocate inside a loop. This way, I hope that
            // allocation will succeed
            _data = (UINT8*)te_allocate(size);
            tryCount.fetch_add(1, std::memory_order_relaxed);

            if (!_data)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }

        } while (_data == nullptr && tryCount < maxTry);

        if (!_data)
        {
            TE_ASSERT_ERROR(_data, "Failed to allocate a buffer of size : " + ToString(size));
        }
        
        _ownsData = true;
    }

    void GpuResourceData::FreeInternalBuffer()
    {
        if(_data == nullptr || !_ownsData)
        {
            return;
        }

        te_free(_data);
        _data = nullptr;
    }

    void GpuResourceData::SetExternalBuffer(UINT8* data)
    {
        FreeInternalBuffer();

        _data = data;
        _ownsData = false;
    }
}

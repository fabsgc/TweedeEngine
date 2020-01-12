#include "Resources/TeGpuResourceData.h"

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
		FreeInternalBuffer();

		_data = (UINT8*)te_allocate(size);
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

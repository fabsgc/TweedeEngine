#include "TeCorePrerequisites.h"
#include "Resources/TeResourceHandle.h"
#include "Resources/TeResource.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    void ResourceHandleBase::Release()
	{
		// TODO
	}

	void ResourceHandleBase::Destroy()
	{
		if(_data->resource)
        {
            // TODO
        }
	}

	void ResourceHandleBase::SetHandleData(const SPtr<Resource>& resource, const UUID& uuid)
	{
		_data->resource = resource;

        if(_data->resource)
        {
            _data->uuid = uuid;
        }
	}

    void ResourceHandleBase::ClearHandleData()
	{
		_data->resource = nullptr;
	}

	void ResourceHandleBase::AddInternalRef()
	{
		_data->refCount++;
	}

	void ResourceHandleBase::RemoveInternalRef()
	{
		_data->refCount--;
	}
}
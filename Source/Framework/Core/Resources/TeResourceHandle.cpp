#include "TeCorePrerequisites.h"
#include "Resources/TeResourceHandle.h"
#include "Resources/TeResource.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    bool ResourceHandleBase::IsLoaded() const
    {
        bool isLoaded = (_handleData != nullptr && _handleData->data != nullptr);
        return isLoaded;
    }

    void ResourceHandleBase::Release()
    {
        gResourceManager().Release(*this);
    }

    void ResourceHandleBase::Destroy()
    {
        if(_handleData->data)
        {
            gResourceManager().Destroy(*this);
        }
    }

    void ResourceHandleBase::SetHandleData(const SPtr<Resource>& resource, const UUID& uuid)
    {
        _handleData->data = resource;
        _handleData->uuid = uuid;
    }

    void ResourceHandleBase::ClearHandleData()
    {
        if (_handleData != nullptr)
        {
            _handleData->data = nullptr;
        }
    }
}

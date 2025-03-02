#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeEvent.h"

namespace te
{
    class TE_CORE_EXPORT ResourceListener
    {
    public:
        ResourceListener();
        virtual ~ResourceListener();
    
    protected:
        /** Called as soon as a resource is modified inside the ResourceManager */
        virtual void OnResourceModified(const HResource& resource) = 0;

        /** Called as soon as a resource is deleted inside the ResourceManager */
        virtual void OnResourceDestroyed(const UUID& uuid, CoreType type) = 0;
    
    protected:
        HEvent _onResourceModified;
        HEvent _onResourceDestroyed;
    };
}

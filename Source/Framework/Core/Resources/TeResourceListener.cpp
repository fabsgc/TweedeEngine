#include "TeResourceListener.h"

#include "Resources/TeResourceManager.h"

namespace te
{
    ResourceListener::ResourceListener()
    {
        _onResourceLoaded = gResourceManager().OnResourceLoaded.Connect(std::bind(&ResourceListener::OnResourceLoaded, this, std::placeholders::_1));
        _onResourceModified = gResourceManager().OnResourceModified.Connect(std::bind(&ResourceListener::OnResourceModified, this, std::placeholders::_1));
        _onResourceDestroyed  = gResourceManager().OnResourceDestroyed.Connect(std::bind(&ResourceListener::OnResourceDestroyed, this, std::placeholders::_1, std::placeholders::_2));
    }

    ResourceListener::~ResourceListener()
    { 
        _onResourceLoaded.Disconnect();
        _onResourceModified.Disconnect();
        _onResourceDestroyed.Disconnect();
    }
}

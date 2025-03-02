#include "TeResourceListener.h"

#include "Resources/TeResourceManager.h"

namespace te
{
    ResourceListener::ResourceListener()
    {
        _onResourceModified = gResourceManager().OnResourceModified.Connect(std::bind(&ResourceListener::OnResourceModified, this, std::placeholders::_1));
        _onResourceDestroyed  = gResourceManager().OnResourceDestroyed.Connect(std::bind(&ResourceListener::OnResourceDestroyed, this, std::placeholders::_1, std::placeholders::_2));
    }

    ResourceListener::~ResourceListener()
    { 
        _onResourceModified.Disconnect();
        _onResourceDestroyed.Disconnect();
    }
}

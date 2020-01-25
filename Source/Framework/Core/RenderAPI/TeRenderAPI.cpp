#include "TeRenderAPI.h"
#include "RenderAPI/TeRenderWindow.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(RenderAPI)

    RenderAPI::RenderAPI()
        : _numDevices(0)
        , _capabilities(nullptr)
    { }

    RenderAPI::~RenderAPI()
    { 
        te_deleteN(_capabilities, _numDevices);
		_capabilities = nullptr;
    }

    void RenderAPI::Initialize()
    { }

    void RenderAPI::Destroy()
    {
        _activeRenderTarget = nullptr;
    }

    const RenderAPICapabilities& RenderAPI::GetCapabilities(UINT32 deviceIdx) const
    {
        if(deviceIdx >= _numDevices)
        {
            TE_DEBUG("Invalid device index provided: {" + ToString(deviceIdx) + "}. Valid range is: [0, {" + ToString(_numDevices) + "}).", __FILE__, __LINE__);
            return _capabilities[0];
        }

        return _capabilities[deviceIdx];
    }
}

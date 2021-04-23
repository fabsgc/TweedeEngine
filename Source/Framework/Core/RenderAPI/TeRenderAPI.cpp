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
            TE_DEBUG("Invalid device index provided: {" + ToString(deviceIdx) + "}. Valid range is: [0, {" + ToString(_numDevices) + "}).");
            return _capabilities[0];
        }

        return _capabilities[deviceIdx];
    }

    RenderAPICapabilities* RenderAPI::GetCapabilitiesPtr(UINT32 deviceIdx) const
    {
        if (deviceIdx >= _numDevices)
        {
            TE_DEBUG("Invalid device index provided: {" + ToString(deviceIdx) + "}. Valid range is: [0, {" + ToString(_numDevices) + "}).");
            return &_capabilities[0];
        }

        return &_capabilities[deviceIdx];
    }

    UINT32 RenderAPI::VertexCountToPrimCount(DrawOperationType type, UINT32 elementCount)
    {
        UINT32 primCount = 0;
        switch (type)
        {
        case DOT_POINT_LIST:
            primCount = elementCount;
            break;

        case DOT_LINE_LIST:
            primCount = elementCount / 2;
            break;

        case DOT_LINE_STRIP:
            primCount = elementCount - 1;
            break;

        case DOT_TRIANGLE_LIST:
            primCount = elementCount / 3;
            break;

        case DOT_TRIANGLE_STRIP:
            primCount = elementCount - 2;
            break;

        case DOT_TRIANGLE_FAN:
            primCount = elementCount - 2;
            break;
        }

        return primCount;
    }
}

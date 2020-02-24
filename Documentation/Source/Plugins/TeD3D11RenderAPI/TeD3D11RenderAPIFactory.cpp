#include "TeD3D11RenderAPIFactory.h"
#include "TeD3D11RenderAPI.h"

namespace te
{
    void D3D11RenderAPIFactory::Create()
    {
        RenderAPI::StartUp<D3D11RenderAPI>();
    }

    const String& D3D11RenderAPIFactory::Name() const
    {
        static String StrSystemName = SystemName;
        return StrSystemName;
    }
}
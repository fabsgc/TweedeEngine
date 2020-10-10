#include "TeD3D11ImGuiAPIFactory.h"
#include "TeD3D11ImGuiAPI.h"

namespace te
{
    SPtr<GuiAPI> D3D11ImGuiAPIFactory::Create()
    {
        GuiAPI::StartUp<D3D11ImGuiAPI>();
        return te_shared_ptr<GuiAPI>(GuiAPI::InstancePtr());
    }

    const String& D3D11ImGuiAPIFactory::Name() const
    {
        static String StrSystemName = SystemName;
        return StrSystemName;
    }
}
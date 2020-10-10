#include "TeGLImGuiAPIFactory.h"
#include "TeGLImGuiAPI.h"

namespace te
{
    SPtr<GuiAPI> GLImGuiAPIFactory::Create()
    {
        GuiAPI::StartUp<GLImGuiAPI>();
        return te_shared_ptr<GuiAPI>(GuiAPI::InstancePtr());
    }

    const String& GLImGuiAPIFactory::Name() const
    {
        static String StrSystemName = SystemName;
        return StrSystemName;
    }
}
#include "TeGLRenderAPIFactory.h"
#include "TeGLRenderAPI.h"

namespace te
{
    void GLRenderAPIFactory::Create()
    {
        RenderAPI::StartUp<GLRenderAPI>();
    }

    const String& GLRenderAPIFactory::Name() const
    {
        static String StrSystemName = SystemName;
        return StrSystemName;
    }
}
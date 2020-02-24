#include "TeRenderManFactory.h"
#include "TeRenderMan.h"

namespace te
{
    SPtr<Renderer> RenderManFactory::Create()
    {
        return te_shared_ptr_new<RenderMan>();
    }

    const String& RenderManFactory::Name() const
    {
        static String StrSystemName = SystemName;
        return StrSystemName;
    }
}
#include "TeLight.h"

namespace te
{
    void Light::Initialize()
    { 
        CoreObject::Initialize();
    }

    void Light::Destroy()
    { }

    void Light::_markCoreDirty(ActorDirtyFlag flag) 
    {
        MarkCoreDirty((UINT32)flag);
    }

    void Light::SetTransform(const Transform& transform)
    {
        SceneActor::SetTransform(transform);
    }
}

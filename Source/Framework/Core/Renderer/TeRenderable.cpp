#include "TeRenderable.h"

namespace te
{
    void Renderable::Initialize()
    { 
        CoreObject::Initialize();
    }

    void Renderable::Destroy()
    { }

    void Renderable::_markCoreDirty(ActorDirtyFlag flag) 
    {
        MarkCoreDirty((UINT32)flag);
    }

    void Renderable::SetTransform(const Transform& transform)
    {
        SceneActor::SetTransform(transform);
    }
}

#include "TeDecal.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    Decal::Decal()
    { }

    Decal::~Decal()
    { }

    void Decal::Initialize()
    { }

    void Decal::_markCoreDirty(ActorDirtyFlag flag) 
    {
        MarkCoreDirty((UINT32)flag);
    }

    void Decal::FrameSync()
    { }

    SPtr<Decal> Decal::Create(const HMaterial& material, const Vector2& size, float maxDistance)
    {
        return nullptr;
    }
}
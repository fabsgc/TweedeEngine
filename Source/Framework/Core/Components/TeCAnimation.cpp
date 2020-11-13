#include "Components/TeCAnimation.h"

namespace te
{
    CAnimation::CAnimation()
        : Component(HSceneObject(), (UINT32)TID_CAnimation)
    {
        _notifyFlags = TCF_Transform;

        SetName("Animation");
        SetFlag(Component::AlwaysRun, true);
    }

    CAnimation::CAnimation(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CAnimation)
    {
        _notifyFlags = TCF_Transform;

        SetName("Animation");
        SetFlag(Component::AlwaysRun, true);
    }

    void CAnimation::_instantiate()
    { }

    void CAnimation::Initialize()
    {
        Component::Initialize();
    }

    void CAnimation::OnInitialized()
    {
        Component::OnInitialized();
    }

    void CAnimation::OnEnabled()
    {
        Component::OnEnabled();
    }

    void CAnimation::OnDisabled()
    {
        Component::OnDisabled();
    }

    void CAnimation::OnDestroyed()
    {
        Component::OnDestroyed();
    }

    void CAnimation::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CAnimation>(c));
    }

    void CAnimation::Clone(const HAnimation& c)
    { }
}

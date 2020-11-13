#include "Components/TeCBone.h"

namespace te
{
    CBone::CBone()
        : Component(HSceneObject(), (UINT32)TID_CBone)
    {
        SetName("Bone");
        SetFlag(Component::AlwaysRun, true);
    }

    CBone::CBone(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CBone)
    {
        SetName("Bone");
        SetFlag(Component::AlwaysRun, true);
    }

    CBone::~CBone()
    { }

    void CBone::_instantiate()
    { }

    void CBone::Initialize()
    {
        Component::Initialize();
    }

    void CBone::OnInitialized()
    {
        Component::OnInitialized();
    }

    void CBone::OnEnabled()
    {
        Component::OnEnabled();
    }

    void CBone::OnDisabled()
    {
        Component::OnDisabled();
    }

    void CBone::OnDestroyed()
    {
        Component::OnDestroyed();
    }

    void CBone::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CBone>(c));
    }

    void CBone::Clone(const HBone& c)
    { }
}

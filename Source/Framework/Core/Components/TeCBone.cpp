#include "Components/TeCBone.h"

#include "Scene/TeSceneObject.h"
#include "Components/TeCAnimation.h"

namespace te
{
    CBone::CBone()
        : Component(HSceneObject(), (UINT32)TID_CBone)
    {
        SetName("Bone");
        SetNotifyFlags(TCF_Parent | TCF_Transform);
        SetFlag(Component::AlwaysRun, true);
    }

    CBone::CBone(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CBone)
    {
        SetName("Bone");
        SetNotifyFlags(TCF_Parent | TCF_Transform);
        SetFlag(Component::AlwaysRun, true);
    }

    void CBone::SetBoneName(const String& name)
    {
        if (_parent == nullptr)
            UpdateParentAnimation();

        if (_boneName == name)
            return;

        _boneName = name;

        if (_parent != nullptr)
            _parent->NotifyBoneChanged(static_object_cast<CBone>(GetHandle()));
    }

    CBone::~CBone()
    { }

    void CBone::Instantiate()
    { }

    void CBone::Initialize()
    {
        Component::Initialize();
    }

    void CBone::OnInitialized()
    {
        UpdateParentAnimation();
        Component::OnInitialized();
    }

    void CBone::OnEnabled()
    {
        UpdateParentAnimation();
        Component::OnEnabled();
    }

    void CBone::OnDisabled()
    {
        if (_parent != nullptr)
            _parent->RemoveBone(static_object_cast<CBone>(GetHandle()));

        _parent = nullptr;

        Component::OnDisabled();
    }

    void CBone::OnTransformChanged(TransformChangedFlags flags)
    {
        if (!SO()->GetActive())
            return;

        if ((flags & TCF_Parent) != 0)
            UpdateParentAnimation();
    }

    void CBone::OnDestroyed()
    {
        if (_parent != nullptr)
            _parent->RemoveBone(static_object_cast<CBone>(GetHandle()));

        _parent = nullptr;

        Component::OnDestroyed();
    }

    void CBone::UpdateParentAnimation()
    {
        HSceneObject currentSO = SO();
        while (currentSO != nullptr)
        {
            HAnimation parent = static_object_cast<CAnimation>(currentSO->GetComponent<CAnimation>());
            if (parent != nullptr)
            {
                if (currentSO->GetActive())
                    SetParent(parent);
                else
                    SetParent(HAnimation());

                return;
            }

            currentSO = currentSO->GetParent();
        }

        SetParent(HAnimation());
    }

    void CBone::SetParent(const HAnimation& animation, bool isInternal)
    {
        if (!isInternal)
        {
            if (!_parent.Empty())
            {
                _parent->RemoveBone(static_object_cast<CBone>(GetHandle()));
                _parent->ForceDirtyState(AnimDirtyStateFlag::Layout);
            }

            if (!animation.Empty())
            {
                animation->AddBone(static_object_cast<CBone>(GetHandle()));
                animation->ForceDirtyState(AnimDirtyStateFlag::Layout);
            }
        }

        _parent = animation;
    }

    bool CBone::Clone(const HBone& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            _boneName = c->_boneName;
            return true;
        }

        return false;
    }
}

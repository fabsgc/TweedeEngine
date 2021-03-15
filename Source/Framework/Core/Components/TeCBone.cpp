#include "Components/TeCBone.h"

#include "Scene/TeSceneObject.h"
#include "Components/TeCAnimation.h"

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

    void CBone::SetBoneName(const String& name)
    {
        if (_parent == nullptr)
            UpdateParentAnimation();

        if (_boneName == name)
            return;

        _boneName = name;

        if (_parent != nullptr)
            _parent->_notifyBoneChanged(static_object_cast<CBone>(GetHandle()));
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
            _parent->_removeBone(static_object_cast<CBone>(GetHandle()));

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
            _parent->_removeBone(static_object_cast<CBone>(GetHandle()));

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
                    _setParent(parent);
                else
                    _setParent(HAnimation());

                return;
            }

            currentSO = currentSO->GetParent();
        }

        _setParent(HAnimation());
    }

    void CBone::_setParent(const HAnimation& animation, bool isInternal)
    {
        if (!isInternal)
        {
            if (!_parent.Empty())
            {
                _parent->_removeBone(static_object_cast<CBone>(GetHandle()));
                _parent->ForceDirtyState(AnimDirtyStateFlag::Layout);
            }

            if (!animation.Empty())
            {
                animation->_addBone(static_object_cast<CBone>(GetHandle()));
                animation->ForceDirtyState(AnimDirtyStateFlag::Layout);
            }
        }

        _parent = animation;
    }

    void CBone::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CBone>(c));
    }

    void CBone::Clone(const HBone& c)
    { 
        Component::Clone(c.GetInternalPtr());

        _boneName = c->_boneName;
    }
}

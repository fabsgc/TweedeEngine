#include "Components/TeCSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCJoint.h"
#include "Physics/TePhysics.h"

namespace te
{
    CSoftBody::CSoftBody()
        : Component(HSceneObject(), (UINT32)TID_CSoftBody)
    {
        SetName("SoftBody");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CSoftBody::CSoftBody(const HSceneObject& parent)
        : Component(parent, (UINT32)TID_CSoftBody)
    {
        SetName("SoftBody");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CSoftBody::~CSoftBody()
    { }

    void CSoftBody::Initialize()
    { 
        OnEnabled();
        Component::Initialize();
    }

    void CSoftBody::Clone(const HComponent& c)
    { 
        Clone(static_object_cast<CSoftBody>(c));
    }

    void CSoftBody::Clone(const HSoftBody& c)
    { 
        Component::Clone(c.GetInternalPtr());
    }

    void CSoftBody::OnInitialized()
    { }

    void CSoftBody::OnDestroyed()
    {
        DestroyInternal();
    }

    void CSoftBody::OnDisabled()
    {
        DestroyInternal();
    }

    void CSoftBody::OnEnabled()
    {
        _internal = SoftBody::Create(SO());
    }

    void CSoftBody::OnTransformChanged(TransformChangedFlags flags)
    {
        if (!SO()->GetActive())
            return;

        if ((flags & TCF_Parent) != 0)
        {

        }

        if (gPhysics().IsUpdateInProgress())
            return;
    }

    void CSoftBody::DestroyInternal()
    {
        if (_internal)
        {
            _internal = nullptr;
        }
    }
}
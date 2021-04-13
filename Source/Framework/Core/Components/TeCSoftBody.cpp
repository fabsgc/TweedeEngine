#include "Components/TeCSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCJoint.h"
#include "Physics/TePhysics.h"

using namespace std::placeholders;

namespace te
{
    CSoftBody::CSoftBody()
        : CBody(HSceneObject(), (UINT32)TID_CSoftBody)
    {
        SetName("SoftBody");
    }

    CSoftBody::CSoftBody(const HSceneObject& parent)
        : CBody(parent, (UINT32)TID_CSoftBody)
    {
        SetName("SoftBody");
    }

    CSoftBody::~CSoftBody()
    { }

    void CSoftBody::Initialize()
    { 
        ClearColliders();
        OnEnabled();
        CBody::Initialize();
    }

    void CSoftBody::Clone(const HComponent& c)
    { 
        Clone(static_object_cast<CSoftBody>(c));
    }

    void CSoftBody::Clone(const HSoftBody& c)
    { 
        CBody::Clone(static_object_cast<CBody>(c));
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
        _internal = CreateInternal();
        UpdateColliders();

#if TE_DEBUG_MODE
        CheckForNestedBody();
#endif

        _internal->OnCollisionBegin.Connect(std::bind(&CSoftBody::TriggerOnCollisionBegin, this, _1));
        _internal->OnCollisionStay.Connect(std::bind(&CSoftBody::TriggerOnCollisionStay, this, _1));
        _internal->OnCollisionEnd.Connect(std::bind(&CSoftBody::TriggerOnCollisionEnd, this, _1));

        const Transform& tfrm = SO()->GetTransform();
        _internal->SetTransform(tfrm.GetPosition(), tfrm.GetRotation());

        // TODO
    }

    void CSoftBody::OnTransformChanged(TransformChangedFlags flags)
    {
        if (!SO()->GetActive())
            return;

        if ((flags & TCF_Parent) != 0)
        {
            ClearColliders();
            UpdateColliders();

#if TE_DEBUG_MODE
            CheckForNestedBody();
#endif
        }

        if (gPhysics().IsUpdateInProgress())
            return;

        const Transform& tfrm = SO()->GetTransform();
        _internal->SetTransform(tfrm.GetPosition(), tfrm.GetRotation());

        // TODO
    }

    SPtr<Body> CSoftBody::CreateInternal()
    {
        SPtr<SoftBody> body = SoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }

    void CSoftBody::DestroyInternal()
    {
        ClearColliders();

        if (_internal)
        {
            _internal->SetOwner(PhysicsOwnerType::None, nullptr);
            _internal = nullptr;
        }
    }

    void CSoftBody::ClearColliders()
    { 
        // TODO
    }

    void CSoftBody::UpdateColliders()
    {
        // TODO
    }

    void CSoftBody::AddCollider(const HCollider& collider)
    {
        // TODO
    }

    void CSoftBody::SyncCollider(const HCollider& collider)
    {
        // TODO
    }

    void CSoftBody::RemoveCollider(const HCollider& collider)
    {
        // TODO
    }

    void CSoftBody::CheckForNestedBody()
    {
        // TODO
    }

    void CSoftBody::ProcessCollisionData(const CollisionDataRaw& raw, CollisionData& output)
    {
        // TODO
    }
}

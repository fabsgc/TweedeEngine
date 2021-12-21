#include "Components/TeCSoftBody.h"

#include "Scene/TeSceneObject.h"
#include "Components/TeCJoint.h"
#include "Physics/TePhysics.h"

#include <functional>

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
        OnEnabled();
        CBody::Initialize();
    }

    bool CSoftBody::Clone(const HSoftBody& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if(CBody::Clone(static_object_cast<CBody>(c), suffix))
        {
            _mesh = c->_mesh;
            _scale = c->_scale;

            return true;
        }

        return false;
    }

    void CSoftBody::SetMesh(const HPhysicsMesh& mesh)
    {
        if (_mesh == mesh)
            return;

        _mesh = mesh;

        if (_internal != nullptr)
            std::static_pointer_cast<SoftBody>(_internal)->SetMesh(mesh);
    }

    void CSoftBody::SetScale(const Vector3& scale)
    {
        if (_scale == scale)
            return;

        _scale = scale;

        if (_internal != nullptr)
            std::static_pointer_cast<SoftBody>(_internal)->SetScale(scale);
    }

    void CSoftBody::Update()
    {
        CBody::Update();
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
        if (_internal != nullptr)
            DestroyInternal();

        _internal = CreateInternal();

        _internal->OnCollisionBegin.Connect(std::bind(&CSoftBody::TriggerOnCollisionBegin, this, _1));
        _internal->OnCollisionStay.Connect(std::bind(&CSoftBody::TriggerOnCollisionStay, this, _1));
        _internal->OnCollisionEnd.Connect(std::bind(&CSoftBody::TriggerOnCollisionEnd, this, _1));

        const Transform& tfrm = SO()->GetTransform();
        _internal->SetTransform(tfrm.GetPosition(), tfrm.GetRotation());

        _internal->SetFriction(_friction);
        _internal->SetRollingFriction(_rollingFriction);
        _internal->SetRestitution(_restitution);
        _internal->SetVelocity(_velocity);
        _internal->SetAngularVelocity(_angularVelocity);
        _internal->SetIsKinematic(_isKinematic);
        _internal->SetIsDebug(_isDebug);
        _internal->SetFlags(_flags);
        _internal->SetCollisionReportMode(_collisionReportMode);
        _internal->SetMass(_mass);

        std::static_pointer_cast<SoftBody>(_internal)->SetMesh(_mesh);
        std::static_pointer_cast<SoftBody>(_internal)->SetScale(_scale);
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

        const Transform& tfrm = SO()->GetTransform();
        _internal->SetTransform(tfrm.GetPosition(), tfrm.GetRotation());
    }

    SPtr<Body> CSoftBody::CreateInternal()
    {
        SPtr<SoftBody> body = SoftBody::Create(SO());
        body->SetOwner(PhysicsOwnerType::Component, this);

        return body;
    }

    void CSoftBody::DestroyInternal()
    {
        if (_internal)
        {
            _internal->SetOwner(PhysicsOwnerType::None, nullptr);
            _internal = nullptr;
        }
    }

    void CSoftBody::ProcessCollisionData(const CollisionDataRaw& raw, CollisionData& output)
    {
        output.ContactPoints = std::move(const_cast<CollisionDataRaw&>(raw).ContactPoints);

        if (raw.Bodies[0] != nullptr)
        {
            CBody* other = (CBody*)raw.Bodies[0]->GetOwner(PhysicsOwnerType::Component);
            output.Bodies[0] = static_object_cast<CBody>(other->GetHandle());
        }

        if (raw.Bodies[1] != nullptr)
        {
            CBody* other = (CBody*)raw.Bodies[1]->GetOwner(PhysicsOwnerType::Component);
            output.Bodies[1] = static_object_cast<CBody>(other->GetHandle());
        }
    }
}

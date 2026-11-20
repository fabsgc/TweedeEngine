#include "Components/TeCBody.h"
#include "Scene/TeSceneObject.h"
#include "Math/TeAABox.h"

namespace te
{
    CBody::CBody(CoreType type)
        : Component(HSceneObject(), type)
        , _internal(nullptr)
    {
        SetName("Body");
        SetNotifyFlags(TCF_Parent | TCF_Transform);
        SetFlag(Component::AlwaysRun, true);
    }

    CBody::CBody(const HSceneObject& parent, CoreType type)
        : Component(parent, type)
        , _internal(nullptr)
    {
        SetName("Body");
        SetNotifyFlags(TCF_Parent | TCF_Transform);
        SetFlag(Component::AlwaysRun, true);
    }

    CBody::~CBody()
    {
        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
    }

    void CBody::Initialize()
    {
        Component::Initialize();
    }

    bool CBody::Clone(const HBody& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            _mass = c->_mass;
            _friction = c->_friction;
            _rollingFriction = c->_rollingFriction;
            _restitution = c->_restitution;
            _isKinematic = c->_isKinematic;
            _isTrigger = c->_isTrigger;
            _isDebug = c->_isDebug;
            _velocity = c->_velocity;
            _angularVelocity = c->_angularVelocity;
            _flags = c->_flags;
            _collisionReportMode = c->_collisionReportMode;

            return true;
        }

        return false;
    }

    void CBody::Update()
    {
        if (_internal != nullptr)
            _internal->Update();
    }

    AABox CBody::GetBoundingBox()
    {
        if (_internal != nullptr)
            return _internal->GetBoundingBox();

        return AABox();
    }

    void CBody::SetIsTrigger(bool trigger)
    {
        if (_isTrigger == trigger)
            return;

        _isTrigger = trigger;

        if (_internal != nullptr)
            _internal->SetIsTrigger(trigger);
    }

    void CBody::SetIsDebug(bool debug)
    {
        if (_isDebug == debug)
            return;

        _isDebug = debug;

        if (_internal != nullptr)
            _internal->SetIsDebug(debug);
    }

    void CBody::SetMass(float mass)
    {
        if (_mass == mass)
            return;

        _mass = mass;

        if (_internal != nullptr)
            _internal->SetMass(mass);
    }

    void CBody::SetIsKinematic(bool kinematic)
    {
        if (_isKinematic == kinematic)
            return;

        _isKinematic = kinematic;

        if (_internal != nullptr)
            _internal->SetIsKinematic(kinematic);
    }

    void CBody::SetVelocity(const Vector3& velocity)
    {
        if (_velocity == velocity)
            return;

        _velocity = velocity;

        if (_internal != nullptr)
            _internal->SetVelocity(velocity);
    }

    void CBody::SetAngularVelocity(const Vector3& velocity)
    {
        if (_angularVelocity == velocity)
            return;

        _angularVelocity = velocity;

        if (_internal != nullptr)
            _internal->SetAngularVelocity(velocity);
    }

    void CBody::SetFriction(float friction)
    {
        if (_friction == friction)
            return;

        _friction = friction;

        if (_internal != nullptr)
            _internal->SetFriction(friction);
    }

    void CBody::SetRollingFriction(float rollingFriction)
    {
        if (_rollingFriction == rollingFriction)
            return;

        _rollingFriction = rollingFriction;

        if (_internal != nullptr)
            _internal->SetRollingFriction(rollingFriction);
    }

    void CBody::SetRestitution(float restitution)
    {
        if (_restitution == restitution)
            return;

        _restitution = restitution;

        if (_internal != nullptr)
            _internal->SetRestitution(restitution);
    }

    void CBody::ApplyForce(const Vector3& force, ForceMode mode) const
    {
        if (_internal != nullptr)
            _internal->ApplyForce(force, mode);
    }

    void CBody::ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const
    {
        if (_internal != nullptr)
            _internal->ApplyForceAtPoint(force, position, mode);
    }

    void CBody::ApplyTorque(const Vector3& torque, ForceMode mode) const
    {
        if (_internal != nullptr)
            _internal->ApplyTorque(torque, mode);
    }

    void CBody::SetCollisionReportMode(CollisionReportMode mode)
    {
        _collisionReportMode = mode;

        if (_internal)
            _internal->SetCollisionReportMode(mode);
    }

    void CBody::SetFlags(BodyFlag flags)
    {
        _flags = flags;

        if (_internal != nullptr)
            _internal->SetFlags(flags);
    }

    void CBody::SetLinkedSO(const HSceneObject& SO)
    {
        if (_internal != nullptr)
            _internal->SetLinkedSO(SO);
    }

    void CBody::CullDebug(bool cull)
    {
        if (_isDebug && _internal)
        {
            _internal->SetIsDebug(cull);
        }
    }

    void CBody::TriggerOnCollisionBegin(const CollisionDataRaw& data)
    {
        CollisionData hit;
        ProcessCollisionData(data, hit);

        OnCollisionBegin(hit);
    }

    void CBody::TriggerOnCollisionStay(const CollisionDataRaw& data)
    {
        CollisionData hit;
        ProcessCollisionData(data, hit);

        OnCollisionStay(hit);
    }

    void CBody::TriggerOnCollisionEnd(const CollisionDataRaw& data)
    {
        CollisionData hit;
        ProcessCollisionData(data, hit);

        OnCollisionEnd(hit);
    }

    void CBody::ExportJson(nlohmann::json& document) const
    {
        Component::ExportJson(document);

        document["mass"] = _mass;
        document["friction"] = _friction;
        document["rollingFriction"] = _rollingFriction;
        document["restitution"] = _restitution;
        document["isKinematic"] = _isKinematic;
        document["isTrigger"] = _isTrigger;
        document["isDebug"] = _isDebug;
        _velocity.ExportJson(document["velocity"]);
        _angularVelocity.ExportJson(document["angularVelocity"]);
        document["bodyFlags"] = static_cast<uint32_t>(_flags);
        document["collisionReportMode"] = static_cast<uint32_t>(_collisionReportMode);
    }

    bool CBody::ImportJson(const nlohmann::json& document, CBody& body)
    {
        Component::ImportJson(document, body);

        if (document.contains("mass"))
            body.SetMass(document["mass"].get<float>());
        if (document.contains("friction"))
            body.SetFriction(document["friction"].get<float>());
        if (document.contains("rollingFriction"))
            body.SetRollingFriction(document["rollingFriction"].get<float>());
        if (document.contains("restitution"))
            body.SetRestitution(document["restitution"].get<float>());
        if (document.contains("isKinematic"))
            body.SetIsKinematic(document["isKinematic"].get<bool>());
        if (document.contains("isTrigger"))
            body.SetIsTrigger(document["isTrigger"].get<bool>());
        if (document.contains("isDebug"))
            body.SetIsDebug(document["isDebug"].get<bool>());
        if (document.contains("velocity"))
            body.SetVelocity(Vector3::ImportJson(document["velocity"]));
        if (document.contains("angularVelocity"))
            body.SetAngularVelocity(Vector3::ImportJson(document["angularVelocity"]));
        if (document.contains("bodyFlags"))
            body.SetFlags(static_cast<BodyFlag>(document["bodyFlags"].get<uint32_t>()));
        if (document.contains("collisionReportMode"))
            body.SetCollisionReportMode(static_cast<CollisionReportMode>(document["collisionReportMode"].get<uint32_t>()));

        return true;
    }
}

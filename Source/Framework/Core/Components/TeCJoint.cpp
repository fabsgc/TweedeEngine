#include "Components/TeCJoint.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "Components/TeCRigidBody.h"
#include "../TeCoreApplication.h"

namespace te
{
    CJoint::CJoint(CoreType type)
        : Component(HSceneObject(), type)
        , _internal(nullptr)
    {
        SetName("Joint");
        SetNotifyFlags(TCF_Parent | TCF_Transform);

        _positions[0] = Vector3::ZERO;
        _positions[1] = Vector3::ZERO;

        _rotations[0] = Quaternion::IDENTITY;
        _rotations[1] = Quaternion::IDENTITY;
    }

    CJoint::CJoint(const HSceneObject& parent, CoreType type)
        : Component(parent, type)
        , _internal(nullptr)
    {
        SetName("Joint");
        SetNotifyFlags(TCF_Parent | TCF_Transform);

        _positions[0] = Vector3::ZERO;
        _positions[1] = Vector3::ZERO;

        _rotations[0] = Quaternion::IDENTITY;
        _rotations[1] = Quaternion::IDENTITY;
    }

    CJoint::~CJoint()
    {
        if (_internal && !_internal->IsDestroyed())
            _internal->Destroy();
    }

    void CJoint::Initialize()
    {
        OnEnabled();
        Component::Initialize();
    }

    bool CJoint::Clone(const HJoint& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            // Note : _bodies should not be copied, as we surely don't want 
            // to apply twice the same constraint of the two same bodies
            // Moreover, in case we are cloning a whole part of the scene
            // We don't want that this joint to be linked to the original
            // SceneObjects

            _breakForce = c->_breakForce;
            _breakTorque = c->_breakTorque;
            _enableCollision = c->_enableCollision;

            _offsetPivots[(int)JointBody::Anchor] = c->_offsetPivots[(int)JointBody::Anchor];
            _offsetPivots[(int)JointBody::Target] = c->_offsetPivots[(int)JointBody::Target];

            return true;
        }

        return false;
    }

    void CJoint::Update()
    {
        bool isRunning = gCoreApplication().GetState().IsFlagSet(ApplicationState::Mode::Physics);
        if (gPhysics().IsPaused() || !isRunning)
            return;

        if (_internal)
            _internal->Update();
    }

    HRigidBody CJoint::GetBody(JointBody body) const
    {
        return _bodies[(int)body];
    }

    void CJoint::SetBody(JointBody body, const HRigidBody& value)
    {
        if (value && value->GetComponentType() != CoreType::TID_CRigidBody)
            return;

        if (_bodies[(int)body] == value)
            return;

        if (_bodies[(int)body] != nullptr)
            _bodies[(int)body]->RemoveJoint(body, static_object_cast<CJoint>(_thisHandle));

        _bodies[(int)body] = value;

        // If joint already exists, destroy it if we removed all bodies, otherwise update its transform
        if (_internal != nullptr)
        {
            if (!IsBodyValid(_bodies[0]) && !IsBodyValid(_bodies[1]))
                DestroyInternal();
            else
            {
                RigidBody* rigidbody = nullptr;
                if (value != nullptr)
                    rigidbody = static_cast<RigidBody*>(value->GetInternal());

                _internal->SetBody(body, rigidbody);
                UpdateTransform(body);
            }
        }
        else // If joint doesn't exist, check if we can create it
        {
            // Must be an active component and at least one of the bodies must be non-null
            if (SO()->GetActive() && (IsBodyValid(_bodies[0]) || IsBodyValid(_bodies[1])))
            {
                RestoreInternal(); 
            }
        }

        if (value != nullptr)
            _bodies[(int)body]->AddJoint(body, static_object_cast<CJoint>(_thisHandle));
    }

    Vector3 CJoint::GetPosition(JointBody body) const
    {
        return _positions[(int)body];
    }

    Quaternion CJoint::GetRotation(JointBody body) const
    {
        return _rotations[(int)body];
    }

    void CJoint::SetTransform(JointBody body, const Vector3& position, const Quaternion& rotation)
    {
        if (_positions[(int)body] == position && _rotations[(int)body] == rotation)
            return;

        _positions[(int)body] = position;
        _rotations[(int)body] = rotation;

        if (_internal != nullptr)
            UpdateTransform(body);
    }

    float CJoint::GetBreakForce() const
    {
        return _breakForce;
    }

    void CJoint::SetBreakForce(float force)
    {
        if (_breakForce == force)
            return;

        _breakForce = force;

        if (_internal != nullptr)
            _internal->SetBreakForce(force);
    }

    float CJoint::GetBreakTorque() const
    {
        return _breakTorque;
    }

    void CJoint::SetBreakTorque(float torque)
    {
        if (_breakTorque == torque)
            return;

        _breakTorque = torque;

        if (_internal != nullptr)
            _internal->SetBreakTorque(torque);
    }

    bool CJoint::GetEnableCollision() const
    {
        return _enableCollision;
    }

    void CJoint::SetEnableCollision(bool value)
    {
        if (_enableCollision == value)
            return;

        _enableCollision = value;

        if (_internal != nullptr)
            _internal->SetEnableCollision(value);
    }

    void CJoint::SetOffsetPivot(JointBody body, const Vector3& offset)
    {
        if (_offsetPivots[(int)body] == offset)
            return;

        _offsetPivots[(int)body] = offset;

        if (_internal != nullptr)
            _internal->SetOffsetPivot(body, offset);
    }

    const Vector3& CJoint::GetOffsetPivot(JointBody body) const
    {
        return _offsetPivots[(int)body];
    }

    void CJoint::SetIsBroken(bool isBroken)
    {
        if (_isBroken == isBroken)
            return;

        _isBroken = isBroken;

        if (!isBroken)
            OnEnabled();
        else
            OnDisabled();

        if (_internal != nullptr)
            _internal->SetIsBroken(isBroken);
    }

    bool CJoint::GetIsBroken()
    {
        return _isBroken;
    }

    void CJoint::OnDestroyed()
    {
        if (_bodies[(int)JointBody::Target] != nullptr)
            _bodies[(int)JointBody::Target]->RemoveJoint(JointBody::Target, static_object_cast<CJoint>(_thisHandle));

        if (_bodies[(int)JointBody::Anchor] != nullptr)
            _bodies[(int)JointBody::Anchor]->RemoveJoint(JointBody::Anchor, static_object_cast<CJoint>(_thisHandle));

        if (_internal != nullptr)
            DestroyInternal();
    }

    void CJoint::OnDisabled()
    { 
        if (_internal != nullptr)
            DestroyInternal();
    }

    void CJoint::OnEnabled()
    {
        if (IsBodyValid(_bodies[0]) || IsBodyValid(_bodies[1]))
            RestoreInternal();

        if (_bodies[(int)JointBody::Anchor] != nullptr)
        {
            _internal->SetBody(JointBody::Anchor, static_cast<RigidBody*>(_bodies[(int)JointBody::Anchor]->GetInternal()));
            _bodies[(int)JointBody::Anchor]->AddJoint(JointBody::Anchor, static_object_cast<CJoint>(_thisHandle));
        }

        if (_bodies[(int)JointBody::Target] != nullptr)
        {
            _internal->SetBody(JointBody::Target, static_cast<RigidBody*>(_bodies[(int)JointBody::Target]->GetInternal()));
            _bodies[(int)JointBody::Target]->AddJoint(JointBody::Target, static_object_cast<CJoint>(_thisHandle));
        }

        if (_internal)
        {
            _internal->SetBreakForce(_breakForce);
            _internal->SetBreakTorque(_breakTorque);
            _internal->SetEnableCollision(_enableCollision);

            _internal->SetOffsetPivot(JointBody::Anchor, _offsetPivots[(int)JointBody::Anchor]);
            _internal->SetOffsetPivot(JointBody::Target, _offsetPivots[(int)JointBody::Target]);
        }
    }

    void CJoint::OnTransformChanged(TransformChangedFlags flags)
    {
        if (_internal == nullptr)
            return;

        // We're ignoring this during physics update because it would cause problems if the joint itself was moved by physics
        // Note: This isn't particularily correct because if the joint is being moved by physics but the rigidbodies
        // themselves are not parented to the joint, the transform will need updating. However I'm leaving it up to the
        // user to ensure rigidbodies are always parented to the joint in such a case (It's an unlikely situation that
        // I can't think of an use for - joint transform will almost always be set as an initialization step and not a
        // physics response).
        if (gPhysics().IsUpdateInProgress())
            return;

        UpdateTransform(JointBody::Target);
        UpdateTransform(JointBody::Anchor);
    }

    void CJoint::RestoreInternal()
    { 
        GetLocalTransform(JointBody::Target, _positions[0], _rotations[0]);
        GetLocalTransform(JointBody::Anchor, _positions[1], _rotations[1]);

        if (_internal != nullptr)
            DestroyInternal();

        _internal = CreateInternal();

        _eventTriggered.Disconnect();
        _eventTriggered = _internal->OnJointBreak.Connect(std::bind(&CJoint::TriggerOnJointBroken, this));
    }

    void CJoint::DestroyInternal()
    {
        if (_bodies[(int)JointBody::Anchor] != nullptr)
            _bodies[(int)JointBody::Anchor]->RemoveJoint(JointBody::Anchor, static_object_cast<CJoint>(_thisHandle));

        if (_bodies[(int)JointBody::Target] != nullptr)
            _bodies[(int)JointBody::Target]->RemoveJoint(JointBody::Target, static_object_cast<CJoint>(_thisHandle));

        // This should release the last reference and destroy the internal joint
        if (_internal)
        {
            _internal->SetOwner(PhysicsOwnerType::None, nullptr);
            if (!_internal->IsDestroyed()) _internal->Destroy();
            _internal = nullptr;
        }

        _eventTriggered.Disconnect();
    }

    void CJoint::GetLocalTransform(JointBody body, Vector3& position, Quaternion& rotation)
    {
        position = _positions[(UINT32)body];
        rotation = _rotations[(UINT32)body];

        HRigidBody rigidbody = _bodies[(UINT32)body];
        if (rigidbody == nullptr) // Get world space transform if no relative to any body
        {
            const Transform& tfrm = SO()->GetTransform();
            Quaternion worldRot = tfrm.GetRotation();

            rotation = worldRot * rotation;
            position = worldRot.Rotate(position) + tfrm.GetPosition();
        }
        else
        {
            position = rotation.Rotate(position);
        }
    }

    void CJoint::NotifyBodyMoved(const HBody& body)
    {
        if (_internal == nullptr)
            return;

        // If physics update is in progress do nothing, as its the joint itself that's probably moving the body
        if (gPhysics().IsUpdateInProgress())
            return;
        
        if (_bodies[0] == body)
            UpdateTransform(JointBody::Target);
        else if (_bodies[1] == body)
            UpdateTransform(JointBody::Anchor);
        else
            assert(false); // Not allowed to happen
    }

    bool CJoint::IsBodyValid(const HRigidBody& body)
    {
        if (body == nullptr)
            return false;

        if (body->GetInternal() == nullptr)
            return false;

        return true;
    }

    void CJoint::UpdateTransform(JointBody body)
    {
        Vector3 localPos;
        Quaternion localRot;
        GetLocalTransform(body, localPos, localRot);

        _internal->SetTransform(body, localPos, localRot);
    }

    void CJoint::TriggerOnJointBroken()
    {
        _isBroken = true;
        OnJointBreak();
        DestroyInternal();
    }

    void CJoint::ExportJson(nlohmann::json& document) const
    {
        Component::ExportJson(document);

        const JointBody anchor = JointBody::Anchor;
        const JointBody target = JointBody::Target;

        document["anchorBody"] = _bodies[(int)anchor] != nullptr ? _bodies[(int)anchor]->GetUUID().ToString() : "";
        document["targetBody"] = _bodies[(int)target] != nullptr ? _bodies[(int)target]->GetUUID().ToString() : "";
        _positions[(int)anchor].ExportJson(document["anchorPosition"]);
        _positions[(int)target].ExportJson(document["targetPosition"]);
        _rotations[(int)anchor].ExportJson(document["anchorRotation"]);
        _rotations[(int)target].ExportJson(document["targetRotation"]);
        _offsetPivots[(int)anchor].ExportJson(document["anchorOffsetPivot"]);
        _offsetPivots[(int)target].ExportJson(document["targetOffsetPivot"]);
        document["breakForce"] = _breakForce;
        document["breakTorque"] = _breakTorque;
        document["enableCollision"] = _enableCollision;
        document["isBroken"] = _isBroken;
    }

    bool CJoint::ImportJson(const nlohmann::json& document, CJoint& joint)
    {
        Component::ImportJson(document, joint);

        if (document.contains("anchorBody") && !document["anchorBody"].get<String>().empty())
            joint._bodyUUIDs[(int)JointBody::Anchor] = UUID(document["anchorBody"].get<String>());
        if (document.contains("targetBody") && !document["targetBody"].get<String>().empty())
            joint._bodyUUIDs[(int)JointBody::Target] = UUID(document["targetBody"].get<String>());

        if (document.contains("anchorPosition"))
            joint.SetTransform(JointBody::Anchor, Vector3::ImportJson(document["anchorPosition"]), joint._rotations[(int)JointBody::Anchor]);
        if (document.contains("targetPosition"))
            joint.SetTransform(JointBody::Target, Vector3::ImportJson(document["targetPosition"]), joint._rotations[(int)JointBody::Target]);
        if (document.contains("anchorRotation"))
            joint.SetTransform(JointBody::Anchor, joint._positions[(int)JointBody::Anchor], Quaternion::ImportJson(document["anchorRotation"]));
        if (document.contains("targetRotation"))
            joint.SetTransform(JointBody::Target, joint._positions[(int)JointBody::Target], Quaternion::ImportJson(document["targetRotation"]));
        if (document.contains("anchorOffsetPivot"))
            joint.SetOffsetPivot(JointBody::Anchor, Vector3::ImportJson(document["anchorOffsetPivot"]));
        if (document.contains("targetOffsetPivot"))
            joint.SetOffsetPivot(JointBody::Target, Vector3::ImportJson(document["targetOffsetPivot"]));
        if (document.contains("breakForce"))
            joint.SetBreakForce(document["breakForce"].get<float>());
        if (document.contains("breakTorque"))
            joint.SetBreakTorque(document["breakTorque"].get<float>());
        if (document.contains("enableCollision"))
            joint.SetEnableCollision(document["enableCollision"].get<bool>());
        if (document.contains("isBroken"))
            joint.SetIsBroken(document["isBroken"].get<bool>());

        return true;
    }

    void CJoint::ResolveBodyReferences(const HSceneObject& sceneRoot)
    {
        bool resolvedBody = false;

        for (UINT32 i = 0; i < 2; i++)
        {
            if (_bodyUUIDs[i].Empty())
                continue;

            HComponent component = sceneRoot->GetComponent(_bodyUUIDs[i], true);
            if (component == nullptr || component->GetCoreType() != CoreType::TID_CRigidBody)
                continue;

            HRigidBody rigidBody = static_object_cast<CRigidBody>(component);
            if (rigidBody->GetInternal() == nullptr)
                continue;

            _bodies[i] = rigidBody;
            _bodyUUIDs[i] = UUID::EMPTY;
            resolvedBody = true;
        }

        if (resolvedBody && !_isBroken)
            OnEnabled();
    }
}

#include "Components/TeCJoint.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "Components/TeCBody.h"

namespace te
{
    CJoint::CJoint(UINT32 type)
        : Component(HSceneObject(), type)
        , _internal(nullptr)
    {
        SetName("Joint");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);

        _positions[0] = Vector3::ZERO;
        _positions[1] = Vector3::ZERO;

        _rotations[0] = Quaternion::IDENTITY;
        _rotations[1] = Quaternion::IDENTITY;
    }

    CJoint::CJoint(const HSceneObject& parent, UINT32 type)
        : Component(parent, type)
        , _internal(nullptr)
    {
        SetName("Joint");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);

        _positions[0] = Vector3::ZERO;
        _positions[1] = Vector3::ZERO;

        _rotations[0] = Quaternion::IDENTITY;
        _rotations[1] = Quaternion::IDENTITY;
    }

    CJoint::~CJoint()
    { }

    void CJoint::Initialize()
    { 
        RestoreInternal();
        OnEnabled();
        Component::Initialize();
    }

    void CJoint::Clone(const HComponent& c)
    { 
        Clone(static_object_cast<CJoint>(c));
    }

    void CJoint::Clone(const HJoint& c)
    { 
        Component::Clone(c.GetInternalPtr());

        _breakForce = c->_breakForce;
        _breakTorque = c->_breakTorque;
        _enableCollision = c->_enableCollision;
    }

    HBody CJoint::GetBody(JointBody body) const
    {
        return _bodies[(int)body];
    }

    void CJoint::SetBody(JointBody body, const HBody& value)
    {
        if (_bodies[(int)body] == value)
            return;

        if (_bodies[(int)body] != nullptr)
            _bodies[(int)body]->RemoveJoint(static_object_cast<CJoint>(_thisHandle));

        _bodies[(int)body] = value;

        // If joint already exists, destroy it if we removed all bodies, otherwise update its transform
        if (_internal != nullptr)
        {
            if (!IsBodyValid(_bodies[0]) && !IsBodyValid(_bodies[1]))
                DestroyInternal();
            else
            {
                Body* rigidbody = nullptr;
                if (value != nullptr)
                    rigidbody = value->GetInternal();

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
            _bodies[(int)body]->AddJoint(static_object_cast<CJoint>(_thisHandle));
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

    void CJoint::OnInitialized()
    { }

    void CJoint::OnDestroyed()
    {
        if (_bodies[0] != nullptr)
            _bodies[0]->RemoveJoint(static_object_cast<CJoint>(_thisHandle));

        if (_bodies[1] != nullptr)
            _bodies[1]->RemoveJoint(static_object_cast<CJoint>(_thisHandle));

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
            _internal->SetBody(JointBody::Anchor, _bodies[(int)JointBody::Anchor]->GetInternal());

        if (_bodies[(int)JointBody::Target] != nullptr)
            _internal->SetBody(JointBody::Target, _bodies[(int)JointBody::Target]->GetInternal());

        _internal->SetBreakForce(_breakForce);
        _internal->SetBreakTorque(_breakTorque);
        _internal->SetEnableCollision(_enableCollision);
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

        _internal = CreateInternal();
        _internal->OnJointBreak.Connect(std::bind(&CJoint::TriggerOnJointBroken, this));
    }

    void CJoint::DestroyInternal()
    { 
        // This should release the last reference and destroy the internal joint
        if (_internal)
        {
            _internal->SetOwner(PhysicsOwnerType::None, nullptr);
            _internal = nullptr;
        }
    }

    void CJoint::GetLocalTransform(JointBody body, Vector3& position, Quaternion& rotation)
    {
        position = _positions[(UINT32)body];
        rotation = _rotations[(UINT32)body];

        HBody rigidbody = _bodies[(UINT32)body];
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

    bool CJoint::IsBodyValid(const HBody& body)
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
        OnJointBreak();
    }
}

#include "TeBulletSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "Physics/TeCollider.h"
#include "Physics/TeJoint.h"
#include "TeBulletPhysics.h"
#include "TeBulletFCollider.h"
#include "TeBulletFBody.h"
#include "TeBulletFJoint.h"
#include "TeBulletJoint.h"
#include "TeBulletFMesh.h"
#include "TeBulletMesh.h"

namespace te
{
    static const float DEFAULT_MASS = 1.0f;
    static const float DEFAULT_FRICTION = 0.5f;
    static const float DEFAULT_ROLLING_FRICTION = 0.0f;
    static const float DEFAULT_RESTITUTION = 0.0f;
    static const float DEFAULT_DEACTIVATION_TIME = 2000;

    BulletSoftBody::BulletSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : SoftBody(linkedSO)
        , _softBody(nullptr)
        , _physics(physics)
        , _scene(scene)
        , _isDirty(false)
    {
        _mass = DEFAULT_MASS;
        _friction = DEFAULT_FRICTION;
        _restitution = DEFAULT_RESTITUTION;
        _rollingFriction = DEFAULT_ROLLING_FRICTION;

        _internal = te_new<BulletFBody>();

        AddToWorld();
    }

    BulletSoftBody::~BulletSoftBody()
    { 
        te_delete((BulletFBody*)_internal);
        Release();
    }

    void BulletSoftBody::Update()
    {
        if (_isDirty && _inWorld)
            AddToWorld();

        _isDirty = false;
    }

    Vector3 BulletSoftBody::GetPosition() const
    {
        if (_softBody)
        {
            if (((UINT32)_flags & (UINT32)BodyFlag::CCD) != 0)
                return ToVector3(_softBody->getInterpolationWorldTransform().getOrigin());
            else
                return ToVector3(_softBody->getWorldTransform().getOrigin());
        }

        return _position;
    }

    Quaternion BulletSoftBody::GetRotation() const
    {
        if (_softBody)
        {
            if (((UINT32)_flags & (UINT32)BodyFlag::CCD) != 0)
                return ToQuaternion(_softBody->getInterpolationWorldTransform().getRotation());
            else
                return ToQuaternion(_softBody->getWorldTransform().getRotation());
        }

        return _rotation;
    }

    void BulletSoftBody::SetMesh(const HPhysicsMesh& mesh)
    {
        if (mesh == _mesh)
            return;

        SoftBody::SetMesh(mesh);
        AddToWorld();
    }

    void BulletSoftBody::SetScale(const Vector3& scale)
    {
        if (scale == _scale)
            return;

        SoftBody::SetScale(scale);
        AddToWorld();
    }

    void BulletSoftBody::SetTransform(const Vector3& pos, const Quaternion& rot)
    {
        _position = pos;
        _rotation = rot;

        if (_softBody)
            _softBody->transformTo(btTransform(ToBtQuaternion(_rotation), ToBtVector3(_position)));

        // Set position and rotation to world transform
        /*const Vector3 oldPosition = GetPosition();
        btTransform& trans = _softBody->getWorldTransform();
        trans.setOrigin(ToBtVector3(pos + ToQuaternion(trans.getRotation()) * _centerOfMass));
        trans.setRotation(ToBtQuaternion(_rotation));

        if (_centerOfMass != Vector3::ZERO)
            trans.setOrigin(ToBtVector3(oldPosition + rot * _centerOfMass));

        // Set position and rotation to interpolated world transform
        btTransform transInterpolated = _softBody->getInterpolationWorldTransform();
        transInterpolated.setRotation(trans.getRotation());
        transInterpolated.setOrigin(trans.getOrigin());

        if (_centerOfMass != Vector3::ZERO)
            transInterpolated.setOrigin(trans.getOrigin());

        _softBody->setInterpolationWorldTransform(transInterpolated);*/
    }

    void BulletSoftBody::SetIsTrigger(bool trigger)
    {
        if (_isTrigger == trigger)
            return;

        _isTrigger = trigger;
        UpdateKinematicFlag();
    }

    void BulletSoftBody::SetIsDebug(bool debug)
    {
        if (_isDebug == debug)
            return;

        _isDebug = debug;
        UpdateKinematicFlag();
    }

    void BulletSoftBody::SetMass(float mass)
    {
        mass = std::max(mass, 0.0f);
        if (mass != _mass)
        {
            _mass = mass;

            if(_softBody)
                _softBody->setTotalMass(_mass);
        }
    }

    void BulletSoftBody::SetIsKinematic(bool kinematic)
    {
        if (kinematic == _isKinematic)
            return;

        _isKinematic = kinematic;
        UpdateKinematicFlag();
    }

    void BulletSoftBody::SetVelocity(const Vector3& velocity)
    {
        if (!_softBody)
            return;

        _velocity = velocity;
        _softBody->setLinearVelocity(ToBtVector3(_velocity));

        if (_velocity != Vector3::ZERO)
            Activate();
    }

    void BulletSoftBody::SetAngularVelocity(const Vector3& velocity)
    {
        if (!_softBody)
            return;

        _angularVelocity = velocity;
        _softBody->setAngularVelocity(ToBtVector3(_angularVelocity));

        if (_angularVelocity != Vector3::ZERO)
            Activate();
    }

    void BulletSoftBody::SetFriction(float friction)
    {
        if (!_softBody)
            return;

        _friction = friction;
        _softBody->setFriction(friction);
    }

    void BulletSoftBody::SetRollingFriction(float rollingFriction)
    {
        if (!_softBody)
            return;

        _rollingFriction = rollingFriction;
        _softBody->setRollingFriction(_rollingFriction);
    }

    void BulletSoftBody::SetRestitution(float restitution)
    {
        if (!_softBody)
            return;

        _restitution = restitution;
        _softBody->setRestitution(restitution);
    }

    void BulletSoftBody::SetFlags(BodyFlag flags)
    {
        if (_flags == flags)
            return;

        _flags = flags;
        UpdateCCDFlag();
    }

    void BulletSoftBody::ApplyForce(const Vector3& force, ForceMode mode) const
    {
        // TODO
    }

    void BulletSoftBody::ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const
    {
        // TODO
    }

    void BulletSoftBody::ApplyTorque(const Vector3& torque, ForceMode mode) const
    {
        // TODO
    }

    void BulletSoftBody::SetCollisionReportMode(CollisionReportMode mode)
    {
        _collisionReportMode = mode;
    }

    void BulletSoftBody::AddToWorld()
    {
        if (_mass < 0.0f)
            _mass = 0.0f;

        Release();

        if (!_mesh.IsLoaded())
            return;

        BulletFMesh* fMesh = static_cast<BulletFMesh*>(_mesh->_getInternal());
        if (!fMesh)
        {
            TE_DEBUG("No data inside the PhysicsMesh");
            return;
        }

        const SPtr<BulletMesh::SoftBodyMesh> softBodyMesh = fMesh->GetSoftBodyMesh();

        if (!softBodyMesh)
        {
            TE_DEBUG("PhysicsMesh does not have any SoftBodyMesh Data");
            return;
        }

        _softBody = _scene->CreateBtSoftBody(softBodyMesh);
        if (_softBody)
        {
            btSoftBody::Material* material = _softBody->appendMaterial();
            material->m_flags -= btSoftBody::fMaterial::DebugDraw;
            material->m_kLST = 1.0;
            _softBody->generateBendingConstraints(2, material);
            _softBody->m_cfg.piterations = 2;
            _softBody->m_cfg.kDF = 1.0;
            _softBody->randomizeConstraints();
            //_softBody->generateClusters(0);
            _softBody->setUserPointer(this);

            //_softBody->m_cfg.collisions = btSoftBody::fCollision::CL_SS +
            //                              btSoftBody::fCollision::CL_RS;

            ((BulletFBody*)_internal)->SetBody(_softBody);

            SetMass(_mass);
            SetScale(_scale);
            SetFriction(_friction);
            SetRestitution(_restitution);
            SetRollingFriction(_rollingFriction);

            if (_mass > 0.0f)
            {
                Activate();
                SetVelocity(_velocity);
                SetAngularVelocity(_angularVelocity);
            }
            else
            {
                SetVelocity(Vector3::ZERO);
                SetAngularVelocity(Vector3::ZERO);
            }

            UpdateKinematicFlag();
            UpdateCCDFlag();

            SetTransform(_position, _rotation);

            _scene->AddSoftBody(_softBody);
            _inWorld = true;
        }
    }

    void BulletSoftBody::Release()
    {
        if (!_softBody)
            return;

        RemoveFromWorld();
        delete _softBody;

        ((BulletFBody*)_internal)->SetBody(nullptr);

        _softBody = nullptr;
    }

    void BulletSoftBody::RemoveFromWorld()
    {
        if (!_softBody)
            return;

        if (_inWorld)
        {
            _softBody->activate(false);
            _scene->RemoveSoftBody(_softBody);
            _inWorld = false;
        }
    }

    void BulletSoftBody::Activate() const
    {
        if (!_softBody)
            return;

        if (_mass > 0.0f)
            _softBody->activate(true);
    }

    bool BulletSoftBody::IsActivated() const
    {
        if (_softBody)
            return _softBody->isActive();

        return false;
    }

    void BulletSoftBody::UpdateKinematicFlag() const
    {
        if (!_softBody)
            return;

        int flags = _softBody->getCollisionFlags();

        if (_isKinematic)
            flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
        else
            flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;

        if (_isTrigger)
            flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
        else
            flags &= ~btCollisionObject::CF_NO_CONTACT_RESPONSE;

        if (_isDebug)
            flags &= ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
        else
            flags |= btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;

        _softBody->setCollisionFlags(flags);
        _softBody->forceActivationState(DISABLE_DEACTIVATION);
        _softBody->setDeactivationTime(DEFAULT_DEACTIVATION_TIME);
    }

    void BulletSoftBody::UpdateCCDFlag() const
    {
        if (!_softBody)
            return;

        if (((UINT32)_flags & (UINT32)BodyFlag::CCD))
        {
            _softBody->setCcdMotionThreshold(0.015f);
            _softBody->setCcdSweptSphereRadius(0.01f);
        }
        else
        {
            _softBody->setCcdMotionThreshold(std::numeric_limits<float>::infinity());
            _softBody->setCcdSweptSphereRadius(0);
        }
    }
}

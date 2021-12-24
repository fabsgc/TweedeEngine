#include "TeBulletMeshSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "Math/TeAABox.h"
#include "TeBulletPhysics.h"
#include "TeBulletFBody.h"
#include "TeBulletFMesh.h"
#include "TeBulletMesh.h"

namespace te
{
    static const float DEFAULT_MASS = 1.0f;
    static const float DEFAULT_FRICTION = 0.5f;
    static const float DEFAULT_ROLLING_FRICTION = 0.2f;
    static const float DEFAULT_RESTITUTION = 0.1f;
    static const float DEFAULT_DEACTIVATION_TIME = 2000;

    BulletMeshSoftBody::BulletMeshSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : MeshSoftBody(linkedSO)
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

    BulletMeshSoftBody::~BulletMeshSoftBody()
    { 
        te_delete((BulletFBody*)_internal);
        Release();
    }

    void BulletMeshSoftBody::Update()
    {
        if (_isDirty && _inWorld)
            AddToWorld();

        _isDirty = false;
    }

    Vector3 BulletMeshSoftBody::GetPosition() const
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

    Quaternion BulletMeshSoftBody::GetRotation() const
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

    AABox BulletMeshSoftBody::GetBoundingBox() const
    {
        return AABox();
    }

    void BulletMeshSoftBody::SetMesh(const HPhysicsMesh& mesh)
    {
        if (mesh == _mesh)
            return;

        _mesh = mesh;
        AddToWorld();
    }

    void BulletMeshSoftBody::SetScale(const Vector3& scale)
    {
        if (_scale == scale)
            return;

        _scale = scale;
        AddToWorld();
    }

    void BulletMeshSoftBody::SetTransform(const Vector3& position, const Quaternion& rotation)
    {
        if (_position == position && _rotation == rotation)
            return;

        _position = position;
        _rotation = rotation;

        if (_softBody)
            _softBody->transformTo(btTransform(ToBtQuaternion(_rotation), ToBtVector3(_position)));
    }

    void BulletMeshSoftBody::SetIsTrigger(bool trigger)
    {
        if (_isTrigger == trigger)
            return;

        _isTrigger = trigger;
        UpdateKinematicFlag();
    }

    void BulletMeshSoftBody::SetIsDebug(bool debug)
    {
        if (_isDebug == debug)
            return;

        _isDebug = debug;
        UpdateKinematicFlag();
    }

    void BulletMeshSoftBody::SetMass(float mass)
    {
        mass = std::max(mass, 0.0f);
        if (mass == _mass)
            return;

        _mass = mass;

        if(_softBody)
            _softBody->setTotalMass(_mass);
    }

    void BulletMeshSoftBody::SetIsKinematic(bool kinematic)
    {
        if (kinematic == _isKinematic)
            return;

        _isKinematic = kinematic;
        UpdateKinematicFlag();
    }

    void BulletMeshSoftBody::SetVelocity(const Vector3& velocity)
    {
        if (_velocity == velocity)
            return;

        _velocity = velocity;

        if (_softBody)
        {
            _softBody->setLinearVelocity(ToBtVector3(_velocity));

            if (_velocity != Vector3::ZERO)
                Activate();
        }
    }

    void BulletMeshSoftBody::SetAngularVelocity(const Vector3& velocity)
    {
        if (_angularVelocity == velocity)
            return;

        _angularVelocity = velocity;

        if (_softBody)
        {
            _softBody->setAngularVelocity(ToBtVector3(_angularVelocity));

            if (_angularVelocity != Vector3::ZERO)
                Activate();
        }
    }

    void BulletMeshSoftBody::SetFriction(float friction)
    {
        if (_friction == friction)
            return;

        _friction = friction;

        if (_softBody)
            _softBody->setFriction(friction);
    }

    void BulletMeshSoftBody::SetRollingFriction(float rollingFriction)
    {
        if (_rollingFriction == rollingFriction)
            return;

        _rollingFriction = rollingFriction;

        if (_softBody)
            _softBody->setRollingFriction(rollingFriction);
    }

    void BulletMeshSoftBody::SetRestitution(float restitution)
    {
        if (_restitution == restitution)
            return;

        _restitution = restitution;

        if (_softBody)
            _softBody->setRestitution(restitution);
    }

    void BulletMeshSoftBody::SetFlags(BodyFlag flags)
    {
        if (_flags == flags)
            return;

        _flags = flags;
        UpdateCCDFlag();
    }

    void BulletMeshSoftBody::ApplyForce(const Vector3& force, ForceMode mode) const
    {
        // TODO
    }

    void BulletMeshSoftBody::ApplyForceAtPoint(const Vector3& force, const Vector3& position, ForceMode mode) const
    {
        // TODO
    }

    void BulletMeshSoftBody::ApplyTorque(const Vector3& torque, ForceMode mode) const
    {
        // TODO
    }

    void BulletMeshSoftBody::SetCollisionReportMode(CollisionReportMode mode)
    {
        _collisionReportMode = mode;
    }

    void BulletMeshSoftBody::AddToWorld()
    {
        if (_mass < 0.0f)
            _mass = 0.0f;

        Release();

        if (!_mesh.IsLoaded())
            return;

        BulletFMesh* fMesh = static_cast<BulletFMesh*>(_mesh->GetInternal());
        if (!fMesh)
        {
            TE_DEBUG("No data inside the PhysicsMesh");
            return;
        }

        const SPtr<BulletMesh::MeshInfo> mesh = fMesh->GetMeshInfo();

        if (!mesh)
        {
            TE_DEBUG("PhysicsMesh does not have any SoftBodyMesh Data");
            return;
        }

        _softBody = _scene->CreateBtSoftBody(mesh);
        if (_softBody)
        {
            btSoftBody::Material* material = _softBody->appendMaterial();
            material->m_flags -= btSoftBody::fMaterial::DebugDraw;
            material->m_kLST = 1.0;

            _softBody->m_cfg.piterations = 2;
            _softBody->m_cfg.kDF = 1.0;
            _softBody->setUserPointer(this);
            _softBody->randomizeConstraints();
            _softBody->generateBendingConstraints(2, material);

            _softBody->scale(ToBtVector3(_scale));
            _softBody->setTotalMass((btScalar)_mass);
            _softBody->setFriction((btScalar)_friction);
            _softBody->setRestitution((btScalar)_restitution);
            _softBody->setRollingFriction((btScalar)_rollingFriction);
            _softBody->transformTo(btTransform(ToBtQuaternion(_rotation), ToBtVector3(_position)));
            
            _softBody->generateClusters(16);
            _softBody->m_cfg.collisions = btSoftBody::fCollision::CL_SS +
                                          btSoftBody::fCollision::CL_RS;

            ((BulletFBody*)_internal)->SetBody(_softBody);

            if (_mass > 0.0f)
            {
                Activate();
                _softBody->setLinearVelocity(ToBtVector3(_velocity));
                _softBody->setAngularVelocity(ToBtVector3(_angularVelocity));
            }
            else
            {
                _softBody->setLinearVelocity(ToBtVector3(Vector3::ZERO));
                _softBody->setAngularVelocity(ToBtVector3(Vector3::ZERO));
            }

            UpdateKinematicFlag();
            UpdateCCDFlag();

            _scene->AddSoftBody(_softBody);
            _inWorld = true;
        }
    }

    void BulletMeshSoftBody::Release()
    {
        if (!_softBody)
            return;

        RemoveFromWorld();
        delete _softBody;

        ((BulletFBody*)_internal)->SetBody(nullptr);

        _softBody = nullptr;
    }

    void BulletMeshSoftBody::RemoveFromWorld()
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

    void BulletMeshSoftBody::Activate() const
    {
        if (!_softBody)
            return;

        if (_mass > 0.0f)
            _softBody->activate(true);
    }

    bool BulletMeshSoftBody::IsActivated() const
    {
        if (_softBody)
            return _softBody->isActive();

        return false;
    }

    void BulletMeshSoftBody::UpdateKinematicFlag() const
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

        for (auto i = 0; i < _softBody->m_faces.size(); i++)
        {
            _softBody->m_faces.at(i).m_material->m_flags = _isDebug ? btSoftBody::fMaterial::DebugDraw : 0;
        }

        _softBody->setCollisionFlags(flags);
        _softBody->forceActivationState(DISABLE_DEACTIVATION);
        _softBody->setDeactivationTime(DEFAULT_DEACTIVATION_TIME);
    }

    void BulletMeshSoftBody::UpdateCCDFlag() const
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

#include "TeBulletMeshSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "TeBulletPhysics.h"
#include "TeBulletFSoftBody.h"
#include "TeBulletFMesh.h"

namespace te
{
    static const float DEFAULT_DEACTIVATION_TIME = 2000;

    BulletMeshSoftBody::BulletMeshSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : MeshSoftBody(linkedSO)
        , _softBody(nullptr)
        , _physics(physics)
        , _scene(scene)
        , _isDirty(false)
    {
        _internal = te_new<BulletFSoftBody>(this);
        AddToWorld();
    }

    BulletMeshSoftBody::~BulletMeshSoftBody()
    { 
        te_delete((BulletFSoftBody*)_internal);
        Release();
    }

    void BulletMeshSoftBody::Update()
    {
        if (_isDirty && _inWorld)
            AddToWorld();

        _isDirty = false;
    }

    void BulletMeshSoftBody::SetMesh(const HPhysicsMesh& mesh)
    {
        if (mesh == _mesh)
            return;

        _mesh = mesh;
        AddToWorld();
    }

    void BulletMeshSoftBody::AddToWorld()
    {
        BulletFSoftBody* fSoftBody = static_cast<BulletFSoftBody*>(_internal);

        if (fSoftBody->GetMass() < 0.0f)
            fSoftBody->SetMass(0.0f);

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

            _softBody->scale(ToBtVector3(static_cast<BulletFSoftBody*>(_internal)->_scale));
            _softBody->setTotalMass((btScalar)fSoftBody->GetMass());
            _softBody->setFriction((btScalar)fSoftBody->GetFriction());
            _softBody->setRestitution((btScalar)fSoftBody->GetRestitution());
            _softBody->setRollingFriction((btScalar)fSoftBody->GetRollingFriction());
            _softBody->transformTo(btTransform(ToBtQuaternion(fSoftBody->GetRotation()), ToBtVector3(fSoftBody->GetPosition())));
            
            _softBody->generateClusters(16);
            _softBody->m_cfg.collisions = btSoftBody::fCollision::CL_SS +
                                          btSoftBody::fCollision::CL_RS;

            ((BulletFSoftBody*)_internal)->SetBtSoftBody(_softBody);
            ((BulletFSoftBody*)_internal)->SetSoftBody(this);

            if (fSoftBody->GetMass() > 0.0f)
            {
                Activate();
                _softBody->setLinearVelocity(ToBtVector3(fSoftBody->GetVelocity()));
                _softBody->setAngularVelocity(ToBtVector3(fSoftBody->GetAngularVelocity()));
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

        ((BulletFSoftBody*)_internal)->SetBtSoftBody(nullptr);
        ((BulletFSoftBody*)_internal)->SetSoftBody(nullptr);

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

        if (static_cast<BulletFSoftBody*>(_internal)->GetMass() > 0.0f)
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

        if (static_cast<BulletFSoftBody*>(_internal)->GetIsKinematic())
            flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
        else
            flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;

        if (static_cast<BulletFSoftBody*>(_internal)->GetIsTrigger())
            flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
        else
            flags &= ~btCollisionObject::CF_NO_CONTACT_RESPONSE;

        if (static_cast<BulletFSoftBody*>(_internal)->GetIsKinematic())
            flags &= ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
        else
            flags |= btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;

        for (auto i = 0; i < _softBody->m_faces.size(); i++)
        {
            _softBody->m_faces.at(i).m_material->m_flags = 
                static_cast<BulletFSoftBody*>(_internal)->GetIsDebug() ? btSoftBody::fMaterial::DebugDraw : 0;
        }

        _softBody->setCollisionFlags(flags);
        _softBody->forceActivationState(DISABLE_DEACTIVATION);
        _softBody->setDeactivationTime(DEFAULT_DEACTIVATION_TIME);
    }

    void BulletMeshSoftBody::UpdateCCDFlag() const
    {
        if (!_softBody)
            return;

        if (((UINT32)static_cast<BulletFSoftBody*>(_internal)->GetFlags() & (UINT32)BodyFlag::CCD))
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

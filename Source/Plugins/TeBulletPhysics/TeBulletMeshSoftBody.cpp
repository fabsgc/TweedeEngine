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
        , BulletSoftBody(physics, scene)
    {
        _internal = te_new<BulletFSoftBody>(this);
        AddToWorld();
    }

    BulletMeshSoftBody::~BulletMeshSoftBody()
    { 
        te_delete((BulletFSoftBody*)_internal);
        Release(_internal);
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

        Release(_internal);

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
                Activate(_internal);
                _softBody->setLinearVelocity(ToBtVector3(fSoftBody->GetVelocity()));
                _softBody->setAngularVelocity(ToBtVector3(fSoftBody->GetAngularVelocity()));
            }
            else
            {
                _softBody->setLinearVelocity(ToBtVector3(Vector3::ZERO));
                _softBody->setAngularVelocity(ToBtVector3(Vector3::ZERO));
            }

            UpdateKinematicFlag(_internal);
            UpdateCCDFlag(_internal);

            _scene->AddSoftBody(_softBody);
            _inWorld = true;
        }
    }
}

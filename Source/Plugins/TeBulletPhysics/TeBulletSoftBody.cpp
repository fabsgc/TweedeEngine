#include "TeBulletSoftBody.h"
#include "TeBulletPhysics.h"
#include "TeBulletFSoftBody.h"

namespace te
{
    static const float DEFAULT_DEACTIVATION_TIME = 2000;

    BulletSoftBody::BulletSoftBody(BulletPhysics* physics, BulletScene* scene)
        : _softBody(nullptr)
        , _physics(physics)
        , _scene(scene)
    { }

    void BulletSoftBody::AddToWorldInternal(FBody* body)
    {
        if (_softBody)
        {
            BulletFSoftBody* fSoftBody = static_cast<BulletFSoftBody*>(body);

            btSoftBody::Material* material = _softBody->appendMaterial();
            material->m_flags -= btSoftBody::fMaterial::DebugDraw;
            material->m_kLST = 1.0;

            _softBody->m_cfg.piterations = 2;
            _softBody->m_cfg.kDF = 1.0;
            _softBody->setUserPointer(this);
            _softBody->randomizeConstraints();
            _softBody->generateBendingConstraints(2, material);

            _softBody->scale(ToBtVector3(fSoftBody->GetScale()));
            _softBody->setTotalMass((btScalar)fSoftBody->GetMass());
            _softBody->setFriction((btScalar)fSoftBody->GetFriction());
            _softBody->setRestitution((btScalar)fSoftBody->GetRestitution());
            _softBody->setRollingFriction((btScalar)fSoftBody->GetRollingFriction());
            _softBody->transformTo(btTransform(ToBtQuaternion(fSoftBody->GetRotation()), ToBtVector3(fSoftBody->GetPosition())));

            _softBody->generateClusters(_numClusters);
            _softBody->m_cfg.collisions = btSoftBody::fCollision::CL_SS +
                btSoftBody::fCollision::CL_RS;

            fSoftBody->SetBtSoftBody(_softBody);
            fSoftBody->SetSoftBody(this);

            if (fSoftBody->GetMass() > 0.0f)
            {
                Activate(fSoftBody);
                _softBody->setLinearVelocity(ToBtVector3(fSoftBody->GetVelocity()));
                _softBody->setAngularVelocity(ToBtVector3(fSoftBody->GetAngularVelocity()));
            }
            else
            {
                _softBody->setLinearVelocity(ToBtVector3(Vector3::ZERO));
                _softBody->setAngularVelocity(ToBtVector3(Vector3::ZERO));
            }

            UpdateKinematicFlag(fSoftBody);
            UpdateCCDFlag(fSoftBody);

            _scene->AddSoftBody(_softBody);
            _inWorld = true;
            _isDirty = false;
        }
    }

    void BulletSoftBody::Release(FBody* fBody)
    {
        if (!_softBody)
            return;

        RemoveFromWorld(fBody);
        delete _softBody;

        ((BulletFSoftBody*)fBody)->SetBtSoftBody(nullptr);
        ((BulletFSoftBody*)fBody)->SetSoftBody(nullptr);

        _softBody = nullptr;
    }

    void BulletSoftBody::RemoveFromWorld(FBody* fBody)
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

    void BulletSoftBody::UpdateKinematicFlag(FBody* fBody) const
    {
        if (!_softBody)
            return;

        int flags = _softBody->getCollisionFlags();

        if (static_cast<BulletFSoftBody*>(fBody)->GetIsKinematic())
            flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
        else
            flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;

        if (static_cast<BulletFSoftBody*>(fBody)->GetIsTrigger())
            flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
        else
            flags &= ~btCollisionObject::CF_NO_CONTACT_RESPONSE;

        if (static_cast<BulletFSoftBody*>(fBody)->GetIsKinematic())
            flags &= ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
        else
            flags |= btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;

        for (auto i = 0; i < _softBody->m_faces.size(); i++)
        {
            _softBody->m_faces.at(i).m_material->m_flags =
                static_cast<BulletFSoftBody*>(fBody)->GetIsDebug() ? btSoftBody::fMaterial::DebugDraw : 0;
        }

        _softBody->setCollisionFlags(flags);
        _softBody->forceActivationState(DISABLE_DEACTIVATION);
        _softBody->setDeactivationTime(DEFAULT_DEACTIVATION_TIME);
    }

    void BulletSoftBody::UpdateCCDFlag(FBody* fBody) const
    {
        if (!_softBody)
            return;

        if (((UINT32)static_cast<BulletFSoftBody*>(fBody)->GetFlags() & (UINT32)BodyFlag::CCD))
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

    void BulletSoftBody::Activate(FBody* fBody) const
    {
        if (!_softBody)
            return;

        if (static_cast<BulletFSoftBody*>(fBody)->GetMass() > 0.0f)
            _softBody->activate(true);
    }

    bool BulletSoftBody::IsActivated(FBody* fBody) const
    {
        if (_softBody)
            return _softBody->isActive();

        return false;
    }
}

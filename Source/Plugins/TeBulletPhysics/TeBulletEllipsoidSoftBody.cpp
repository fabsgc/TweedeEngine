#include "TeBulletEllipsoidSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "TeBulletPhysics.h"
#include "TeBulletFSoftBody.h"
#include "TeBulletFMesh.h"

namespace te
{
    static const float DEFAULT_DEACTIVATION_TIME = 2000;

    BulletEllipsoidSoftBody::BulletEllipsoidSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : EllipsoidSoftBody(linkedSO)
        , _softBody(nullptr)
        , _physics(physics)
        , _scene(scene)
        , _isDirty(false)
    {
        _internal = te_new<BulletFSoftBody>(this);
        AddToWorld();
    }

    BulletEllipsoidSoftBody::~BulletEllipsoidSoftBody()
    {
        te_delete((BulletFSoftBody*)_internal);
        Release();
    }

    void BulletEllipsoidSoftBody::Update()
    {
        if (_isDirty && _inWorld)
            AddToWorld();

        _isDirty = false;
    }

    void BulletEllipsoidSoftBody::AddToWorld()
    {
        BulletFSoftBody* fSoftBody = static_cast<BulletFSoftBody*>(_internal);

        if (fSoftBody->GetMass() < 0.0f)
            fSoftBody->SetMass(0.0f);

        Release();

        // TODO
    }

    void BulletEllipsoidSoftBody::Release()
    {
        if (!_softBody)
            return;

        RemoveFromWorld();
        delete _softBody;

        ((BulletFSoftBody*)_internal)->SetBtSoftBody(nullptr);
        ((BulletFSoftBody*)_internal)->SetSoftBody(nullptr);

        _softBody = nullptr;
    }

    void BulletEllipsoidSoftBody::RemoveFromWorld()
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

    void BulletEllipsoidSoftBody::Activate() const
    {
        if (!_softBody)
            return;

        if (static_cast<BulletFSoftBody*>(_internal)->GetMass() > 0.0f)
            _softBody->activate(true);
    }

    bool BulletEllipsoidSoftBody::IsActivated() const
    {
        if (_softBody)
            return _softBody->isActive();

        return false;
    }

    void BulletEllipsoidSoftBody::UpdateKinematicFlag() const
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

    void BulletEllipsoidSoftBody::UpdateCCDFlag() const
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

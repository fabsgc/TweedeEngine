#include "TeBulletEllipsoidSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "TeBulletPhysics.h"
#include "TeBulletFSoftBody.h"
#include "TeBulletFMesh.h"

namespace te
{
    BulletEllipsoidSoftBody::BulletEllipsoidSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : EllipsoidSoftBody(linkedSO)
        , BulletSoftBody(physics, scene)
    {
        _internal = te_new<BulletFSoftBody>(this);
        AddToWorld();
    }

    BulletEllipsoidSoftBody::~BulletEllipsoidSoftBody()
    {
        te_delete((BulletFSoftBody*)_internal);
        Release(_internal);
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

        Release(_internal);

        // TODO
    }
}

#include "TeBulletPatchSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "TeBulletPhysics.h"
#include "TeBulletFSoftBody.h"
#include "TeBulletFMesh.h"

namespace te
{
    BulletPatchSoftBody::BulletPatchSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : PatchSoftBody(linkedSO)
        , BulletSoftBody(physics, scene)
    {
        _internal = te_new<BulletFSoftBody>(this);
        AddToWorld();
    }

    BulletPatchSoftBody::~BulletPatchSoftBody()
    {
        te_delete((BulletFSoftBody*)_internal);
        Release(_internal);
    }

    void BulletPatchSoftBody::Update()
    {
        if (_isDirty && _inWorld)
            AddToWorld();

        _isDirty = false;
    }

    void BulletPatchSoftBody::AddToWorld()
    {
        BulletFSoftBody* fSoftBody = static_cast<BulletFSoftBody*>(_internal);

        if (fSoftBody->GetMass() < 0.0f)
            fSoftBody->SetMass(0.0f);

        Release(_internal);

        // TODO
    }
}

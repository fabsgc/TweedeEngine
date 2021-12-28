#include "TeBulletRopeSoftBody.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"
#include "TeBulletPhysics.h"
#include "TeBulletFSoftBody.h"
#include "TeBulletFMesh.h"

namespace te
{
    BulletRopeSoftBody::BulletRopeSoftBody(BulletPhysics* physics, BulletScene* scene, const HSceneObject& linkedSO)
        : RopeSoftBody(linkedSO)
        , BulletSoftBody(physics, scene)
    {
        _internal = te_new<BulletFSoftBody>(this);
        _numClusters = 0;
        AddToWorld();
    }

    BulletRopeSoftBody::~BulletRopeSoftBody()
    {
        te_delete((BulletFSoftBody*)_internal);
        Release(_internal);
    }

    void BulletRopeSoftBody::Update()
    {
        if (_isDirty && _inWorld)
            AddToWorld();

        _isDirty = false;
    }

    void BulletRopeSoftBody::SetFrom(const Vector3& from)
    {
        _from = from;
    }

    Vector3 BulletRopeSoftBody::GetFrom() const
    {
        return _from;
    }

    void BulletRopeSoftBody::SetTo(const Vector3& to)
    {
        _to = to;
    }

    Vector3 BulletRopeSoftBody::GetTo() const
    {
        return _to;
    }

    void BulletRopeSoftBody::SetResolution(UINT32 resolution)
    {
        _resolution = resolution;
    }

    UINT32 BulletRopeSoftBody::GetResolution() const
    {
        return _resolution;
    }

    void BulletRopeSoftBody::AddToWorld()
    {
        BulletFSoftBody* fSoftBody = static_cast<BulletFSoftBody*>(_internal);

        if (fSoftBody->GetMass() < 0.0f)
            fSoftBody->SetMass(0.0f);

        Release(_internal);

        _softBody = _scene->CreateBtSoftBodyFromRope(_from, _to, _resolution, 1);
        AddToWorldInternal(_internal);
    }
}

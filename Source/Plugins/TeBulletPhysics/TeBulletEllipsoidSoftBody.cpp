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
        UpdateInternal(_internal);

        if (_isDirty && _inWorld)
            AddToWorld();

        _isDirty = false;
    }

    void BulletEllipsoidSoftBody::SetCenter(const Vector3& center)
    {
        _center = center;

        AddToWorld();
    }

    Vector3 BulletEllipsoidSoftBody::GetCenter() const
    {
        return _center;
    }

    void BulletEllipsoidSoftBody::SetRadius(const Vector3& radius)
    {
        _radius = radius;

        AddToWorld();
    }

    Vector3 BulletEllipsoidSoftBody::GetRadius() const
    {
        return _radius;
    }

    void BulletEllipsoidSoftBody::SetResolution(UINT32 resolution)
    {
        _resolution = resolution;

        AddToWorld();
    }

    UINT32 BulletEllipsoidSoftBody::GetResolution() const
    {
        return _resolution;
    }

    void BulletEllipsoidSoftBody::AddToWorld()
    {
        BulletFSoftBody* fSoftBody = static_cast<BulletFSoftBody*>(_internal);

        if (fSoftBody->GetMass() < 0.0f)
            fSoftBody->SetMass(0.0f);

        Release(_internal);

        _softBody = _scene->CreateBtSoftBodyFromEllipsoid(_center, _radius, _resolution);
        AddToWorldInternal(_internal);
    }
}

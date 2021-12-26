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
        , _topLeft(-0.5f, 0.0f, 0.5f)
        , _topRight(0.5f, 0.0f, 0.5f)
        , _bottomLeft(-0.5f, 0.0f, -0.5f)
        , _bottomRight(0.5f, 0.0f, -0.5f)
        , _resolutionX(8)
        , _resolutionY(8)
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

    void BulletPatchSoftBody::SetCorners(const Vector3& topLeft, const Vector3& topRight, const Vector3& bottomLeft, const Vector3& bottomRight)
    {
        _topLeft = topLeft;
        _topRight = topRight;
        _bottomLeft = bottomLeft;
        _bottomRight = bottomRight;

        AddToWorld();
    }

    void BulletPatchSoftBody::GetCorners(Vector3& topLeft, Vector3& topRight, Vector3& bottomLeft, Vector3& bottomRight)
    {
        topLeft = _topLeft;
        topRight = _topRight;
        bottomLeft = _bottomLeft;
        bottomRight = _bottomRight;
    }

    void BulletPatchSoftBody::SetResolution(UINT32 x, UINT32 y)
    {
        _resolutionX = x;
        _resolutionY = y;

        AddToWorld();
    }

    void BulletPatchSoftBody::GetResolution(UINT32& x, UINT32& y)
    {
        x = _resolutionX;
        y = _resolutionY;
    }

    void BulletPatchSoftBody::AddToWorld()
    {
        BulletFSoftBody* fSoftBody = static_cast<BulletFSoftBody*>(_internal);

        if (fSoftBody->GetMass() < 0.0f)
            fSoftBody->SetMass(0.0f);

        Release(_internal);

        _softBody = _scene->CreateBtSoftBodyFromPatch(_topLeft, _topRight, _bottomLeft, _bottomRight, _resolutionX, _resolutionY);
        AddToWorldInternal(_internal);
    }
}

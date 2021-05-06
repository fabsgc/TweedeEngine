#include "TeBulletHeightField.h"
#include "Image/TeTexture.h"
#include "TeBulletPhysics.h"

namespace te
{
    BulletHeightField::BulletHeightField(const SPtr<Texture>& texture)
        : PhysicsHeightField(texture)
    { }

    void BulletHeightField::Initialize()
    {
        if (_internal == nullptr) // Could be not-null if we're deserializing
            _internal = te_shared_ptr_new<BulletFHeightField>(_initTexture);

        PhysicsHeightField::Initialize();
    }

    void BulletHeightField::Destroy()
    {
        _internal = nullptr;
        PhysicsHeightField::Destroy();
    }

    BulletFHeightField::BulletFHeightField()
        : FPhysicsHeightField(nullptr, TID_FBulletHeightField)
    { }

    BulletFHeightField::BulletFHeightField(const SPtr<Texture>& texture)
        : FPhysicsHeightField(texture, TID_FBulletHeightField)
    {
        Initialize();
    }

    BulletFHeightField::~BulletFHeightField()
    {

    }

    void BulletFHeightField::Initialize()
    {

    }
}

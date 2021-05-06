#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysicsHeightField.h"

namespace te
{
    /** Bullet implementation of a PhysicsHeightField. */
    class BulletHeightField : public PhysicsHeightField
    {
    public:
        BulletHeightField(const SPtr<Texture>& texture);

    private:
        /** @copydoc PhysicsHeightField::Initialize() */
        void Initialize() override;

        /** @copydoc PhysicsHeightField::Destroy() */
        void Destroy() override;
    };

    /** Bullet implementation of the PhysicsHeightField foundation, FPhysicsHeightField. */
    class BulletFHeightField : public FPhysicsHeightField
    {
    public:
        BulletFHeightField(const SPtr<Texture>& texture);
        ~BulletFHeightField();

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TID_FBulletHeightField; }

    private:
        /** Creates the internal height field representation */
        void Initialize();

    public:
        BulletFHeightField(); // Serialization only
    };
}

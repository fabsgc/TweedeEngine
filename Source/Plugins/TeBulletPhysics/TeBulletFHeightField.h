#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TeFPhysicsHeightField.h"
#include "TeBulletHeightField.h"

namespace te
{
    /** Bullet implementation of the PhysicsHeightField foundation, FPhysicsHeightField. */
    class BulletFHeightField : public FPhysicsHeightField
    {
    public:
        BulletFHeightField(const SPtr<Texture>& texture);
        ~BulletFHeightField();

        /** Returns height field data */
        SPtr<BulletHeightField::HeightFieldInfo> GetHeightFieldInfo() const { return _heightFieldInfo; }

    private:
        /** Creates the internal height field representation */
        void Initialize();

    public:
        BulletFHeightField(); // Serialization only

    private:
        SPtr<BulletHeightField::HeightFieldInfo> _heightFieldInfo = nullptr;
    };
}

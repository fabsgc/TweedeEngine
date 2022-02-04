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
        virtual ~BulletFHeightField();

        /** Returns height field data */
        SPtr<BulletHeightField::HeightFieldInfo> GetHeightFieldInfo() const { return _heightFieldInfo; }

    protected:
        BulletFHeightField(); // Serialization only

        /** Creates the internal height field representation */
        void Initialize();

    private:
        SPtr<BulletHeightField::HeightFieldInfo> _heightFieldInfo = nullptr;
    };
}

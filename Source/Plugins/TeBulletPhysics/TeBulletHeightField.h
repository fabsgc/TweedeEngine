#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysicsHeightField.h"

namespace te
{
    /** Bullet implementation of a PhysicsHeightField. */
    class BulletHeightField : public PhysicsHeightField
    {
    public:
        struct HeightFieldInfo
        {
            UINT8* HeightMap = nullptr;
            UINT32 Width = 0;
            UINT32 Length = 0;
            float MinHeight = FLT_MAX;
            float MaxHeight = -FLT_MAX;

            HeightFieldInfo(UINT32 width, UINT32 length);
            ~HeightFieldInfo();

            float& GetHeightAt(UINT32 x, UINT32 y) const;
            void SetHeightAt(UINT32 x, UINT32 y, const float& value);
        };

    public:
        BulletHeightField(const SPtr<Texture>& texture);

    private:
        /** @copydoc PhysicsHeightField::Initialize */
        void Initialize() override;

        /** @copydoc PhysicsHeightField::Destroy */
        void Destroy() override;
    };
}

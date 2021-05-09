#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysicsHeightField.h"

#include <cfloat>

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

            float& GetHeightAt(UINT32 x, UINT y) const;
            void SetHeightAt(UINT32 x, UINT y, const float& value);
        };

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

#include "TeBulletHeightField.h"
#include "TeBulletFHeightField.h"

namespace te
{
    BulletHeightField::HeightFieldInfo::HeightFieldInfo(UINT32 width, UINT32 length)
        : Width(width)
        , Length(length)
    {
        HeightMap = te_allocate<UINT8>(sizeof(float) * Width * Length);
    }

    BulletHeightField::HeightFieldInfo::~HeightFieldInfo()
    {
        te_safe_delete(HeightMap);
        HeightMap = nullptr;
    }

    float& BulletHeightField::HeightFieldInfo::GetHeightAt(UINT32 x, UINT32 y) const
    {
        UINT32 offset = y * Width + x;
        return *((float*)HeightMap + offset);
    }

    void BulletHeightField::HeightFieldInfo::SetHeightAt(UINT32 x, UINT32 y, const float& value)
    {
        UINT32 offset = y * Width + x;
        *((float*)HeightMap + offset) = value;

        if (value > MaxHeight)
            MaxHeight = value;

        if (value < MinHeight)
            MinHeight = value;
    }

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
}

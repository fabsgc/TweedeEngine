#include "TeBulletHeightField.h"
#include "TeBulletPhysics.h"
#include "Image/TeTexture.h"
#include "Image/TeColor.h"

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
        _heightFieldInfo = nullptr;
    }

    void BulletFHeightField::Initialize()
    {
        _heightFieldInfo = te_shared_ptr_new<BulletHeightField::HeightFieldInfo>(_texture->GetProperties().GetWidth(), 
            _texture->GetProperties().GetHeight());

        const TextureProperties& properties = _texture->GetProperties();
        SPtr<PixelData> PixelData = _texture->GetProperties().AllocBuffer(0, 0);

        if (properties.GetUsage() & TU_CPUCACHED)
            _texture->ReadCachedData(*PixelData);
        else
            _texture->ReadData(*PixelData);

        for (UINT32 j = 0; j < _heightFieldInfo->Length; j++)
        {
            for (UINT32 i = 0; i < _heightFieldInfo->Width; i++)
            {
                Color color = PixelData->GetColorAt(i, j);
                _heightFieldInfo->SetHeightAt(i, j, color.r);
            }
        }
    }
}

#include "TeBulletFHeightField.h"
#include "TeBulletHeightField.h"
#include "Image/TeTexture.h"
#include "Image/TeColor.h"

namespace te
{
    BulletFHeightField::BulletFHeightField(const SPtr<Texture>& texture)
        : FPhysicsHeightField(texture)
    {
        Initialize();
    }

    BulletFHeightField::BulletFHeightField()
        : FPhysicsHeightField(nullptr)
    { }

    BulletFHeightField::~BulletFHeightField()
    {
        _heightFieldInfo = nullptr;
    }

    void BulletFHeightField::Initialize()
    {
        if (!_texture)
        {
            TE_DEBUG("Provided Texture is invalid.");
            return;
        }

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

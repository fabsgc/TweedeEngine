#include "TeBulletHeightField.h"
#include "TeBulletPhysics.h"
#include "Image/TeTexture.h"
#include "Image/TeColor.h"

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
        const TextureProperties& properties = _texture->GetProperties();
        SPtr<PixelData> pixelData = _texture->GetProperties().AllocBuffer(0, 0);
        
        if (properties.GetUsage() & TU_CPUCACHED)
            _texture->ReadCachedData(*pixelData);
        else
            _texture->ReadData(*pixelData);

        UINT32 width = std::min(pixelData->GetWidth(), pixelData->GetHeight());

        for (UINT32 j = 0; j < width; j++)
        {
            for (UINT32 i = 0; i < width; i++)
            {
                Color color = pixelData->GetColorAt(i, j);
            }
        }
    }
}

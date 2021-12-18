#include "Physics/TePhysicsHeightField.h"
#include "Resources/TeResourceManager.h"
#include "Physics/TePhysics.h"
#include "Image/TeTexture.h"

namespace te
{
    PhysicsHeightField::PhysicsHeightField(const SPtr<Texture>& texture)
        : Resource(TID_PhysicsHeightField)
        , _initTexture(texture)
    {
        // Derived class is responsible for initializing _internal
    }

    SPtr<Texture> PhysicsHeightField::GetTexture() const
    {
        return _internal->GetTexture();
    }

    HPhysicsHeightField PhysicsHeightField::Create(const SPtr<Texture>& texture)
    {
        if (texture)
        {
            SPtr<PhysicsHeightField> newHeightField = CreatePtr(texture);
            return static_resource_cast<PhysicsHeightField>(gResourceManager()._createResourceHandle(newHeightField));
        }
        
        return HPhysicsHeightField();
    }

    HPhysicsHeightField PhysicsHeightField::Create(const HTexture& texture)
    {
        if (texture.IsLoaded())
        {
            SPtr<PhysicsHeightField> newHeightField = CreatePtr(texture.GetInternalPtr());
            return static_resource_cast<PhysicsHeightField>(gResourceManager()._createResourceHandle(newHeightField));
        }

        return HPhysicsHeightField();
    }

    SPtr<PhysicsHeightField> PhysicsHeightField::CreatePtr(const SPtr<Texture>& texture)
    {
        if (texture)
        {
            SPtr<PhysicsHeightField> newHeightField = gPhysics().CreateHeightField(texture);
            newHeightField->SetThisPtr(newHeightField);
            newHeightField->Initialize();

            return newHeightField;
        }

        return nullptr;
    }

    void PhysicsHeightField::Initialize()
    {
        _initTexture = nullptr;
        Resource::Initialize();
    }

    FPhysicsHeightField::FPhysicsHeightField(const SPtr<Texture>& texture, UINT32 TID_type)
        : Serializable(TID_type)
        , _texture(texture)
    { }
}

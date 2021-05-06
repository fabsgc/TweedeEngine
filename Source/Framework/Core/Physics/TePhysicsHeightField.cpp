#include "Physics/TePhysicsHeightField.h"
#include "Resources/TeResourceManager.h"
#include "Physics/TePhysics.h"

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
        SPtr<PhysicsHeightField> newMesh = _createPtr(texture);
        return static_resource_cast<PhysicsHeightField>(gResourceManager()._createResourceHandle(newMesh));
    }

    SPtr<PhysicsHeightField> PhysicsHeightField::_createPtr(const SPtr<Texture>& texture)
    {
        SPtr<PhysicsHeightField> newMesh = gPhysics().CreateHeightField(texture);
        newMesh->SetThisPtr(newMesh);
        newMesh->Initialize();

        return newMesh;
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

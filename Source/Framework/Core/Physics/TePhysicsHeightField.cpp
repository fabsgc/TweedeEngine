#include "Physics/TePhysicsHeightField.h"
#include "Physics/TeFPhysicsHeightField.h"
#include "Resources/TeResourceManager.h"
#include "Physics/TePhysics.h"
#include "Image/TeTexture.h"

namespace te
{
    PhysicsHeightField::PhysicsHeightField(const SPtr<Texture>& texture)
        : Resource(CoreType::TID_PhysicsHeightField)
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

    SPtr<PhysicsHeightField> PhysicsHeightField::CreateEmpty()
    {
        SPtr<PhysicsHeightField> heightField = gPhysics().CreateHeightField(nullptr);
        if (heightField)
            heightField->SetThisPtr(heightField);

        return heightField;
    }

    void PhysicsHeightField::Initialize()
    {
        Resource::Initialize();
        // _initTexture = nullptr; not sure for serialization purpose ?
    }

    void PhysicsHeightField::Serialize(StreamWriter* serializer) const
    {
        Resource::Serialize(serializer);

        SPtr<Texture> texture = _internal ? _internal->GetTexture() : _initTexture;
        TE_ASSERT_ERROR(texture != nullptr, "Cannot serialize a physics height field without a texture.");
        if (!texture)
            return;

        serializer->WriteString(texture->GetUUID().ToString());
    }

    bool PhysicsHeightField::Deserialize(StreamReader* deserializer, PhysicsHeightField* object)
    {
        if (!deserializer || !object)
            return false;

        if (!Resource::Deserialize(deserializer, object))
            return false;

        String textureUUID;
        deserializer->ReadString(textureUUID);
        if (!deserializer->IsStreamGood() || textureUUID.empty())
            return false;

        HTexture texture = gResourceManager().Get<Texture>(UUID(textureUUID));
        if (!texture.IsLoaded())
        {
            TE_DEBUG("Failed to load PhysicsHeightField texture: " + textureUUID);
            return false;
        }

        object->_initTexture = texture.GetInternalPtr();
        object->Initialize();

        return object->_internal != nullptr;
    }
}

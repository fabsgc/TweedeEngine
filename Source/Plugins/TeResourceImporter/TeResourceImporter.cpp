#include "TeResourceImporter.h"

#include "Importer/TeResourceImportOptions.h"

#include "Serialization/TeBinaryReader.h"
#include "Animation/TeAnimationClip.h"
#include "Audio/TeAudioClip.h"
#include "Image/TeTexture.h"
#include "Material/TeMaterial.h"
#include "Material/TeShader.h"
#include "Mesh/TeMesh.h"
#include "Physics/TePhysicsHeightField.h"
#include "Physics/TePhysicsMesh.h"
#include "Scripting/TeScript.h"
#include "Text/TeFont.h"

#include <filesystem>

namespace te
{ 
    ResourceImporter::ResourceImporter()
        : BaseImporter()
    {
        _extensions.push_back("resource");
    }

    bool ResourceImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    template<typename T>
    SPtr<T> DeserializeOneResource(const std::filesystem::path& resourcePath)
    {
        SPtr<T> resource = T::CreateEmpty();

        BinaryReader* deserializer = te_new<BinaryReader>(resourcePath);

        T::Deserialize(deserializer, resource.get());
        return resource;
    }

    SPtr<Resource> ResourceImporter::Import(const String& filePath, const ImportOptions& importOptions)
    {
        const ResourceImportOptions& resourceImportOptions = static_cast<const ResourceImportOptions&>(importOptions);

        switch (resourceImportOptions.ResourceType)
        {
        case TID_AnimationClip:
            return DeserializeOneResource<AnimationClip>(filePath);

        case TID_AudioClip:
            return DeserializeOneResource<AudioClip>(filePath);

        case TID_Texture:
            return DeserializeOneResource<Texture>(filePath);

        case TID_Material:
            return DeserializeOneResource<Material>(filePath);

        case TID_Shader:
            return DeserializeOneResource<Shader>(filePath);

        case TID_Mesh:
            return DeserializeOneResource<Mesh>(filePath);

        case TID_ZPrepassMesh:
            return DeserializeOneResource<ZPrepassMesh>(filePath);

        case TID_PhysicsHeightField:
            return DeserializeOneResource<PhysicsHeightField>(filePath);

        case TID_PhysicsMesh:
            return DeserializeOneResource<PhysicsMesh>(filePath);

        case TID_Script:
            return DeserializeOneResource<Script>(filePath);

        case TID_Font:
            return DeserializeOneResource<Font>(filePath);

        default:
            TE_DEBUG("Undefined resource type");
            break;
        }

        return nullptr;
    }
}

#include "TeProjectImporter.h"

#include "Project/TeProject.h"
#include "Importer/TeProjectImportOptions.h"
#include "Serialization/TeBinaryReader.h"
#include "Utility/TeDataStream.h"
#include "Resources/TeResourceManager.h"

#include "Material/TeMaterial.h"

//#include <ranges>
#include <iostream>
#include <filesystem>

namespace te
{ 
    ProjectImporter::ProjectImporter()
        : BaseImporter()
    {
        _extensions.push_back(u8"project");
    }

    bool ProjectImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ImportOptions> ProjectImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<ProjectImportOptions>();
    }

    template<typename T>
    SPtr<T> DeserializeOneResource(const std::filesystem::path& resourcePath)
    {
        SPtr<T> resource = T::CreateEmpty();
        BinaryReader* deserializer = te_new<BinaryReader>(resourcePath);

        T::Deserialize(deserializer, resource.get());

        return resource;
    }

    SPtr<Resource> ProjectImporter::Import(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        SPtr<Project> project = nullptr;
        std::filesystem::path projectPath = std::filesystem::absolute(filePath);
        std::filesystem::path workingDirectory = projectPath.parent_path();
        const ProjectImportOptions* projectImportOptions = static_cast<const ProjectImportOptions*>(importOptions.get());

        if (!std::filesystem::exists(projectPath))
            return project;

        project = Project::CreatePtr();

        BinaryReader* deserializer = te_new<BinaryReader>(projectPath);
        project->Deserialize(deserializer, project.get());

        Vector<String> resourceNames = project->GetAllResourceNames();
        project->ClearResources();

        Resource* resourceMetaData = new Resource(TID_Resource);

        for (const auto& name : resourceNames)
        {
            std::filesystem::path resourcePath = workingDirectory;
            resourcePath += std::filesystem::path::preferred_separator;
            resourcePath += "resources";
            resourcePath += std::filesystem::path::preferred_separator;
            resourcePath += name;

            BinaryReader* resourceDeserializer = te_new<BinaryReader>(resourcePath);
            Resource::Deserialize(resourceDeserializer, resourceMetaData);

            if (!gResourceManager().Get(resourceMetaData->GetUUID()).IsLoaded())
            {
                SPtr<Resource> resource = nullptr;

                switch (resourceMetaData->GetCoreType())
                {
                case TID_Material:
                    resource = DeserializeOneResource<Material>(resourcePath);
                    gResourceManager().RegisterEngineResource(resourcePath.generic_string(), resource);
                break;

                default:
                    break;
                }

                if (resource)
                {
                    project->AddResource(resource.get());
                }
            }

            te_delete(resourceDeserializer);
        }

        te_delete(resourceMetaData);
        te_delete(deserializer);

        return project;
    }
}

#include "TeProjectImporter.h"

#include "Project/TeProject.h"
#include "Importer/TeProjectImportOptions.h"
#include "Importer/TeResourceImportOptions.h"
#include "Serialization/TeBinaryReader.h"
#include "Serialization/TeUtility.h"
#include "Resources/TeResourceManager.h"

#include <iostream>
#include <filesystem>

namespace te
{ 
    ProjectImporter::ProjectImporter()
        : BaseImporter()
    {
        _extensions.push_back("project");
    }

    bool ProjectImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<Resource> ProjectImporter::Import(const String& filePath, const ImportOptions& importOptions)
    {
        SPtr<Project> project = Project::CreateEmpty();
        const std::filesystem::path projectPath = std::filesystem::absolute(filePath);
        const std::filesystem::path workingDirectory = projectPath.parent_path();
        const ProjectImportOptions& projectImportOptions = static_cast<const ProjectImportOptions&>(importOptions);

        if (!std::filesystem::exists(projectPath))
            return project;

        Project* projectPtr = project.get();
        BinaryReader* deserializer = te_new<BinaryReader>(projectPath);
        Project::Deserialize(deserializer, &projectPtr);

        Vector<String> resourceNames = project->GetAllResourceNames();
        project->ClearResources();

        Resource* resourceMetaData = new Resource(TID_Resource);

        for (const auto& name : resourceNames)
        {
            const std::filesystem::path resourcePath = serialization::GetProjectResourcePath(workingDirectory, name);
            if (!std::filesystem::exists(resourcePath))
            {
                TE_DEBUG("Resource with path \"" + resourcePath.generic_string() + "\" does not exist.");
                continue;
            }

            BinaryReader* resourceDeserializer = te_new<BinaryReader>(resourcePath);
            Resource::Deserialize(resourceDeserializer, resourceMetaData);

            ResourceImportOptions importOptions;
            importOptions.ResourceType = static_cast<TypeID_Core>(resourceMetaData->GetCoreType());

            te_delete(resourceDeserializer);

            HResource resource = gResourceManager().Load<Resource>(resourcePath.generic_string(), importOptions);
            if (resource.IsLoaded())
            {
                project->AddResource(resource.GetInternalPtr().get());
                TE_DEBUG("Resource imported from the specified path : " + resource->GetPath());
            }
            else
            {
                TE_DEBUG("Failed to import the resource from the specified path : " + resourcePath.generic_string());
            }
        }

        resourceMetaData->Destroy();

        te_delete(resourceMetaData);
        te_delete(deserializer);

        return project;
    }
}

#include "TeProjectExporter.h"

#include "ThirdParty/Slugify/slugify.hpp"
#include "Exporter/TeProjectExportOptions.h"
#include "Exporter/TeResourceExportOptions.h"
#include "Serialization/TeBinaryWriter.h"
#include "Serialization/TeUtility.h"
#include "Project/TeProject.h"
#include "Utility/TeFileSystem.h"

#include <filesystem>
#

namespace te
{ 
    ProjectExporter::ProjectExporter()
        : BaseExporter()
    {
        _extensions.push_back("project");
    }

    bool ProjectExporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    bool ProjectExporter::Export(void* object, const String& filePath, const ExportOptions& exportOptions, bool force)
    {
        const std::filesystem::path projectPath = std::filesystem::path(filePath);
        const std::filesystem::path workingDirectory = projectPath.parent_path();
        const ProjectExportOptions& projectExportOptions = static_cast<const ProjectExportOptions&>(exportOptions);

        Project* project = static_cast<Project*>(object);
        BinaryWriter* serializer = te_new<BinaryWriter>(projectPath);

        project->SetPath(filePath);
        project->Serialize(serializer);

        const std::filesystem::path resourcesPath = serialization::GetProjectResourcesPath(workingDirectory);
        FileSystem::CreateDirectory(resourcesPath.generic_string());

        for (auto& resource : project->GetAllResources())
        {
            const std::filesystem::path resourcePath = serialization::GetProjectResourcePath(workingDirectory, resource);

            if (gExporter().Export(resource, resourcePath.generic_string(), ResourceExportOptions()))
            {
                TE_DEBUG("Resource saved at the specified path : " + resource->GetPath());
            }
            else
            {
                TE_DEBUG("Failed to save the resource project at the specified path : " + resource->GetPath());
                return false;
            }
        }

        te_delete(serializer);

        return true;
    }
}

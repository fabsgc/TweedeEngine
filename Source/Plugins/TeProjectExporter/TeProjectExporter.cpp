#include "TeProjectExporter.h"

#include "ThirdParty/Slugify/slugify.hpp"
#include "Exporter/TeProjectExportOptions.h"
#include "Exporter/TeResourceExportOptions.h"
#include "Serialization/TeBinaryWriter.h"
#include "Serialization/TeUtility.h"
#include "Project/TeProject.h"
#include "Utility/TeFileSystem.h"

#include <system_error>

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

    bool ProjectExporter::Export(void* object, const std::filesystem::path& filePath, const ExportOptions& exportOptions, bool force)
    {
        const std::filesystem::path workingDirectory = filePath.parent_path();
        const ProjectExportOptions& projectExportOptions = static_cast<const ProjectExportOptions&>(exportOptions);

        Project* project = static_cast<Project*>(object);
        BinaryWriter* serializer = te_new<BinaryWriter>(filePath);

        const std::filesystem::path resourcesPath = serialization::GetProjectResourcesPath(workingDirectory);
        std::error_code error;
        std::filesystem::remove_all(resourcesPath, error);
        
        if (error == std::error_condition())
            std::filesystem::create_directory(resourcesPath);
        
        project->SetPath(filePath);
        project->Serialize(serializer);

        for (auto& resource : project->GetAllResources())
        {
            const std::filesystem::path resourcePath = serialization::GetProjectResourcePath(workingDirectory, resource);

            if (gExporter().Export(resource, resourcePath, ResourceExportOptions()))
            {
                TE_DEBUG("Resource saved at the specified path : " + resource->GetPath().generic_string());
            }
            else
            {
                TE_DEBUG("Failed to save the resource project at the specified path : " + resource->GetPath().generic_string());
                return false;
            }
        }

        te_delete(serializer);

        return true;
    }
}

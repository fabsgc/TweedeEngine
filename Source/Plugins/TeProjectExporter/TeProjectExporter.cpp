#include "TeProjectExporter.h"

#include "ThirdParty/Slugify/slugify.hpp"
#include "Exporter/TeProjectExportOptions.h"
#include "Serialization/TeBinaryWriter.h"
#include "Project/TeProject.h"
#include "Utility/TeFileSystem.h"

#include <filesystem>

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

    SPtr<ExportOptions> ProjectExporter::CreateExportOptions() const
    {
        return te_shared_ptr_new<ProjectExportOptions>();
    }

    bool ProjectExporter::Export(void* object, const String& filePath, SPtr<const ExportOptions> exportOptions, bool force)
    {
        const std::filesystem::path projectPath = std::filesystem::path(filePath);
        const std::filesystem::path workingDirectory = projectPath.parent_path();
        const ProjectExportOptions* projectExportOptions = static_cast<const ProjectExportOptions*>(exportOptions.get());

        Project* project = static_cast<Project*>(object);
        BinaryWriter* serializer = te_new<BinaryWriter>(projectPath);

        project->SetPath(filePath);
        project->Serialize(serializer);

        for (auto& resource : project->GetAllResources())
        {
            const std::string name = slugify(resource->GetName());
            std::filesystem::path resourcePath = workingDirectory;
            resourcePath += std::filesystem::path::preferred_separator;
            resourcePath += "resources";
            resourcePath += std::filesystem::path::preferred_separator;
            resourcePath += name + ".resource";

            FileSystem::CreateDir(resourcePath.parent_path().generic_string());

            BinaryWriter* resourceSerializer = te_new<BinaryWriter>(resourcePath);
            resource->SetPath(resourcePath.generic_string());
            resource->Serialize(resourceSerializer);

            te_delete(resourceSerializer);
        }

        te_delete(serializer);

        return true;
    }
}

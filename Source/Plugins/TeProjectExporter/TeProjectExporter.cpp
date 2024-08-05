#include "TeProjectExporter.h"

#include "Exporter/TeProjectExportOptions.h"
#include "Serialization/TeBinarySerializer.h"
#include "Project/TeProject.h"

#include <filesystem>

namespace te
{ 
    ProjectExporter::ProjectExporter()
        : BaseExporter()
    {
        _extensions.push_back(u8"project");
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
        std::filesystem::path projectPath = std::filesystem::path(filePath);
        std::filesystem::path workingDirectory = projectPath.parent_path();

        Project* project = static_cast<Project*>(object);
        const ProjectExportOptions* projectExportOptions = static_cast<const ProjectExportOptions*>(exportOptions.get());
        BinarySerializer* serializer = te_new<BinarySerializer>(std::filesystem::path(projectPath));

        project->Serialize(serializer);

        for (auto& resource : project->GetAllResources())
        {
            std::string name = resource->GetName();
            std::filesystem::path resourcePath = workingDirectory;
            resourcePath += std::filesystem::path::preferred_separator;
            
            ToLowerCase(name);
            name = ReplaceAll(name, " ", "-");
            name = ReplaceAll(name, ".", "-");
            name = ReplaceAll(name, "\\", "-");
            name = ReplaceAll(name, "/", "-");
            name = ReplaceAll(name, "*", "");
            name = ReplaceAll(name, "+", "");
            name = ReplaceAll(name, "*", "");

            resourcePath += name + ".resource";

            BinarySerializer* resourceSerializer = te_new<BinarySerializer>(resourcePath);
            resource->Serialize(resourceSerializer);

            te_delete(resourceSerializer);
        }

        te_delete(serializer);

        return true;
    }
}

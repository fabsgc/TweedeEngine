#include "TeProjectImporter.h"

#include "Importer/TeProjectImportOptions.h"
#include "Serialization/TeBinaryReader.h"
#include "Project/TeProject.h"

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

    SPtr<Resource> ProjectImporter::Import(const std::filesystem::path& filePath, const ImportOptions& importOptions)
    {
        SPtr<Project> project = Project::CreateEmpty();
        const std::filesystem::path projectPath = std::filesystem::absolute(filePath);
        const std::filesystem::path workingDirectory = projectPath.parent_path();
        const ProjectImportOptions& projectImportOptions = static_cast<const ProjectImportOptions&>(importOptions);

        if (!std::filesystem::exists(projectPath))
            return project;

        BinaryReader* deserializer = te_new<BinaryReader>(projectPath);
        Project::Deserialize(deserializer, project.get(), workingDirectory);

        te_delete(deserializer);

        return project;
    }
}

#include "TeProjectImporter.h"
#include "Project/TeProject.h"
#include "Importer/TeProjectImportOptions.h"

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

    SPtr<Resource> ProjectImporter::Import(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        SPtr<Project> project = nullptr;
        auto path = std::filesystem::absolute(filePath);
        const ProjectImportOptions* projectImportOptions = static_cast<const ProjectImportOptions*>(importOptions.get());

        if (std::filesystem::exists(path))
        {
            project = Project::CreatePtr();

            project->SetName(path.filename().generic_string());
            project->SetPath(path.generic_string());
        }

        return project;
    }
}

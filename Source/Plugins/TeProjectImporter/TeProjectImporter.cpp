#include "TeProjectImporter.h"
#include "Project/TeProject.h"
#include "Importer/TeProjectImportOptions.h"
#include "Utility/TeDataStream.h"

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

    SPtr<Resource> ProjectImporter::Import(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        SPtr<Project> project = nullptr;
        auto path = std::filesystem::absolute(filePath);
        const ProjectImportOptions* projectImportOptions = static_cast<const ProjectImportOptions*>(importOptions.get());

        if (std::filesystem::exists(path))
        {
            project = Project::CreatePtr();

            nlohmann::json jsonDocument;
            FileStream file(filePath);

            if (file.Fail())
            {
                TE_ASSERT_ERROR(false, "Cannot open file: " + filePath);
                return nullptr;
            }

            size_t size = file.Size();

            if (size > std::numeric_limits<UINT32>::max())
            {
                TE_ASSERT_ERROR(false, "File size larger than supported!");
            }

            uint8_t* data = static_cast<uint8_t*>(te_allocate(static_cast<UINT32>(size) + 1));
            memset(data, 0, size);
            file.Read(static_cast<char*>((void*)data), static_cast<std::streamsize>(size));
            data[size] = (uint8_t)'\0';

            String dataStr((char*)data);

#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND)) && !defined(JSON_NOEXCEPTION)
            try
            {
                jsonDocument = nlohmann::json::parse(dataStr);
            }
            catch (...)
            {
                TE_ASSERT_ERROR(false, "Can't read shader file " + filePath);
            }
#else
            jsonDocument = nlohmann::json::parse(dataStr);
#endif
            te_delete(data);

            project->SetName(path.filename().generic_string());
            project->SetPath(path.generic_string());
        }

        return project;
    }
}

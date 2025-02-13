#pragma once

#include "TeProjectImporterPrerequisites.h"
#include "Importer/TeBaseImporter.h"
#include "Importer/TeImporter.h"
#include "Json/json.h"

namespace te
{
    /** Importer implementation that handles scene import. */
    class ProjectImporter : public BaseImporter
    {
    public:
        ProjectImporter();
        virtual ~ProjectImporter() = default;

        /** @copydoc BaseImporter::IsExtensionSupported */
        bool IsExtensionSupported(const String& ext) const override;

        /** @copydoc SpecificImporter::import */
        SPtr<Resource> Import(const std::filesystem::path& filePath, const ImportOptions& importOptions) override;

    private:
        Vector<String> _extensions;
    };
}

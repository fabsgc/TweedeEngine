#include "Importer/TeBaseImporter.h"

namespace te
{
    Vector<SubResourceRaw> BaseImporter::ImportAll(const String& filePath, const ImportOptions& importOptions)
    {
        SPtr<Resource> resource = Import(filePath, importOptions);
        if (resource == nullptr)
            return Vector<SubResourceRaw>();

        return { { "primary", resource } };
    }
}

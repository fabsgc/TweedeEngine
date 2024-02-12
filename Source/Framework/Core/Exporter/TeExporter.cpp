#include "Exporter/TeExporter.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Exporter)

    Exporter::Exporter()
    { }

    Exporter::~Exporter()
    { }

    void Exporter::RegisterAssetExporter(BaseExporter* exporter)
    {
        if (!exporter)
        {
            TE_ASSERT_ERROR((exporter == nullptr), "Trying to register a null asset exporter!");
            return;
        }

        _assetExporters.push_back(exporter);
    }

    TE_CORE_EXPORT Exporter& gExporter()
    {
        return Exporter::Instance();
    }
}

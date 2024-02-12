#pragma once

#include "TeCorePrerequisites.h"
#include "Exporter/TeExportOptions.h"
#include "Exporter/TeBaseExporter.h"
#include "Utility/TeModule.h"

namespace te
{
    /** Module responsible for exporting various asset types used internally in the engine. */
    class TE_CORE_EXPORT Exporter : public Module<Exporter>
    {
    public:
        Exporter(); 
        virtual ~Exporter();

        TE_MODULE_STATIC_HEADER_MEMBER(Exporter)

        /**
         * Registers a new asset exporter for a specific set of extensions (as determined by the implementation). If an
         * asset exporter for one or multiple extensions already exists, it is removed and replaced with this one.
         * @param[in]	exporter	The exporter that is able to handle import of certain type of files.
         *
         * @note	This method should only be called by asset exporters themselves on startup. Exporter takes ownership
         *			of the provided pointer and will release it. Assumes it is allocated using the general allocator.
         */
        void RegisterAssetExporter(BaseExporter* exporter);

    private:
        Vector<BaseExporter*> _assetExporters;
    };

    /** Provides easier access to Exporter. */
    TE_CORE_EXPORT Exporter& gExporter();
}

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
         * Exports a resource at the specified location, and returns true for success, false otherwise.
         *
         * @param[in]	inputFilePath	Pathname of the input file.
         * @param[in]	exportOptions	(optional) Options for controlling the export. Caller must ensure export options
         *								actually match the type of the exporter used for the file type.
         * @return						boolean : true for success, false othewise
        */
        bool Export(const String& inputFilePath, SPtr<const ExportOptions> exportOptions);

        /**
         * Checks if we can export a file with the specified extension.
         *
         * @param[in]	extension	The extension without the leading dot.
         */
        bool SupportsFileType(const String& extension) const;

        /**
         * Registers a new asset exporter for a specific set of extensions (as determined by the implementation). If an
         * asset exporter for one or multiple extensions already exists, it is removed and replaced with this one.
         * @param[in]	exporter	The exporter that is able to handle export of certain type of files.
         *
         * @note	This method should only be called by asset exporters themselves on startup. Exporter takes ownership
         *			of the provided pointer and will release it. Assumes it is allocated using the general allocator.
         */
        void RegisterAssetExporter(BaseExporter* exporter);

    private:
        /**
         * Searches available exporters and attempts to find one that can export the file of the provided type. Returns null
         * if one cannot be found.
         */
        BaseExporter* GetExporterForFile(const String& inputFilePath) const;

        /**
         * Prepares for export of a file at the specified path. Returns the type of exporter the file can be exported with,
         * or null if the file isn't valid or is of unsupported type. Also creates the default set of export options unless
         * already provided.
         */
        BaseExporter* PrepareForExport(const String& filePath, SPtr<const ExportOptions>& exportOptions) const;

    private:
        Vector<BaseExporter*> _assetExporters;
    };

    /** Provides easier access to Exporter. */
    TE_CORE_EXPORT Exporter& gExporter();
}

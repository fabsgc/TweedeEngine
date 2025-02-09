#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "Exporter/TeExportOptions.h"

namespace te
{
    /**
     * Abstract class that needs to be specialized for converting assets used inside the engine to file
     *
     * On initialization this class must register itself with the Exporter module, which delegates asset export calls to a
     * specific exporter.
     */

    class TE_CORE_EXPORT BaseExporter
    {
    public:
        BaseExporter() = default;
        virtual ~BaseExporter() = default;

        /**
         * Check is the provided extension supported by this exporter.
         * @note	Provided extension should be without the leading dot.
         */
        virtual bool IsExtensionSupported(const String& ext) const = 0;

        /**
         * Export the resource to the given file.
         *
         * @param[in]	filePath		Pathname of the file, with file extension.
         * @param[in]	exportOptions	Options that can control how is the resource exported.
         * @return						bool (true if the export was successfull, false instead)
         */
        virtual bool Export(void* object, const String& filePath, const ExportOptions& exportOptions, bool force = true) = 0;
    };
}

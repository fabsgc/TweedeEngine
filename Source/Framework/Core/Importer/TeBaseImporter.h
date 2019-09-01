#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "Importer/TeImportOptions.h"

namespace te
{
	/**
	 * Abstract class that needs to be specialized for converting a certain asset type into an engine usable resource
	 * (for example a .png file into an engine usable texture).
	 * 			
	 * On initialization this class must register itself with the Importer module, which delegates asset import calls to a 
	 * specific importer.
	 */
	class TE_CORE_EXPORT BaseImporter
	{
	public:
		BaseImporter() = default;
        virtual ~BaseImporter() = default;

        /**
         * Check is the provided extension supported by this importer.
         * @note	Provided extension should be without the leading dot.
         */
        virtual bool IsExtensionSupported(const String& ext) const = 0;

        /**
         * Imports the given file. If file contains more than one resource only the primary resource is imported (for
         * example for an FBX a mesh would be imported, but animations ignored).
         *
         * @param[in]	filePath		Pathname of the file, with file extension.
         * @param[in]	importOptions	Options that can control how is the resource imported.
         * @return						null if it fails, otherwise the loaded object.
         */
        virtual SPtr<Resource> Import(const String& filePath, SPtr<const ImportOptions> importOptions) = 0;

        /**
         * Creates import options specific for this importer. Import options are provided when calling import() in order
         * to customize the import, and provide additional information.
         */
        virtual SPtr<ImportOptions> CreateImportOptions() const;

        /**
         * Gets the default import options.
         * @return	The default import options.
         */
        SPtr<const ImportOptions> GetDefaultImportOptions() const;

    private:
        mutable SPtr<const ImportOptions> _defaultImportOptions;
    };
}
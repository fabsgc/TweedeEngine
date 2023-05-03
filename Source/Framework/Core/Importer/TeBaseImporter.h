#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "Importer/TeImportOptions.h"

namespace te
{
    /**
     * Contains a resource that was imported from a file that contains multiple resources (for example an animation from an
     * FBX file).
     */
    struct SubResourceRaw
    {
        String Name; /**< Unique name of the sub-resource. */
        SPtr<Resource> Res; /**< Contents of the sub-resource. */
    };

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
        virtual SPtr<Resource> Import(const String& filePath, const SPtr<const ImportOptions> importOptions) = 0;

        /**
         * Imports the given file. This method returns all imported resources, which is relevant for files that can contain
         * multiple resources (for example an FBX which may contain both a mesh and animations).
         *
         * @param[in]	filePath		Pathname of the file, with file extension.
         * @param[in]	importOptions	Options that can control how are the resources imported.
         * @return						Empty array if it fails, otherwise the loaded objects. First element is always the
         *								primary resource.
         */
        virtual Vector<SubResourceRaw> ImportAll(const String& filePath, SPtr<const ImportOptions> importOptions);

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

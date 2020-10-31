#include "Importer/TeImporter.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Importer)

    Importer::Importer()
    {}

    Importer::~Importer()
    {
        for (auto i = _assetImporters.begin(); i != _assetImporters.end(); ++i)
        {
            if ((*i) != nullptr)
            {
                te_delete(*i);
            }
        }

        _assetImporters.clear();
    }

    SPtr<Resource> Importer::_import(const String& inputFilePath, SPtr<const ImportOptions> importOptions)
    {
        BaseImporter* importer = PrepareForImport(inputFilePath, importOptions);
        if (importer == nullptr)
        {
            return nullptr;
        }

        SPtr<Resource> output = importer->Import(inputFilePath, importOptions);

        return output;
    }

    HResource Importer::Import(const String& inputFilePath, SPtr<const ImportOptions> importOptions, const UUID& uuid)
    {
        SPtr<Resource> importedResource = _import(inputFilePath, importOptions);

        if (importedResource)
        {
            TE_DEBUG("Resource " + inputFilePath + " has been successfully loaded");
            return gResourceManager()._createResourceHandle(importedResource, uuid);
        }
        else
        {
            TE_DEBUG("Resource " + inputFilePath + " has not been loaded");
        }
        
        return HResource();
    }

    bool Importer::SupportsFileType(const String& extension) const
    {
        for (auto iter = _assetImporters.begin(); iter != _assetImporters.end(); ++iter)
        {
            if (*iter != nullptr && (*iter)->IsExtensionSupported(extension))
            {
                return true;
            }
        }

        return false;
    }

    void Importer::_registerAssetImporter(BaseImporter* importer)
    {
        if (!importer)
        {
            TE_ASSERT_ERROR((importer == nullptr), "Trying to register a null asset importer!");
            return;
        }

        _assetImporters.push_back(importer);
    }

    BaseImporter* Importer::GetImporterForFile(const String& inputFilePath) const
    {
        String ext = Util::GetFileExtension(inputFilePath);
        
        if (ext.empty())
            return nullptr;

        ext = ext.substr(1, ext.size() - 1); // Remove the .
        if (!SupportsFileType(ext))
        {
            TE_ASSERT_ERROR(false, "There is no importer for the provided file" + inputFilePath);
            return nullptr;
        }

        for (auto iter = _assetImporters.begin(); iter != _assetImporters.end(); ++iter)
        {
            if (*iter != nullptr && (*iter)->IsExtensionSupported(ext))
            {
                return *iter;
            }
        }

        return nullptr;
    }

    BaseImporter* Importer::PrepareForImport(const String& filePath, SPtr<const ImportOptions>& importOptions) const
    {
        BaseImporter* importer = GetImporterForFile(filePath);
        if (importer == nullptr)
            return nullptr;

        if (importOptions == nullptr)
        {
            importOptions = importer->GetDefaultImportOptions();
        }
        else
        {
            SPtr<const ImportOptions> defaultImportOptions = importer->GetDefaultImportOptions();
        }

        return importer;
    }

    TE_CORE_EXPORT Importer& gImporter()
    {
        return Importer::Instance();
    }
}
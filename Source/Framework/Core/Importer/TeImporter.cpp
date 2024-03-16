#include "Importer/TeImporter.h"
#include "Resources/TeResourceManager.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(Importer)

    Importer::Importer()
    {}

    Importer::~Importer()
    {
        for (const auto& assetImporters : _assetImporters)
        {
            te_safe_delete(assetImporters);
        }
    }

    SPtr<Resource> Importer::_import(const String& inputFilePath, SPtr<const ImportOptions> importOptions)
    {
        BaseImporter* importer = PrepareForImport(inputFilePath, importOptions);
        if (!importer)
            return nullptr;

        SPtr<Resource> output = importer->Import(inputFilePath, importOptions);
        return output;
    }

    HResource Importer::Import(const String& inputFilePath, SPtr<const ImportOptions> importOptions, const UUID& uuid)
    {
        SPtr<Resource> importedResource = _import(inputFilePath, importOptions);

        if (importedResource)
        {
            TE_DEBUG("Resource from " + inputFilePath + " has been successfully loaded");
            return gResourceManager()._createResourceHandle(importedResource, uuid);
        }
        else
        {
            TE_DEBUG("Resource from " + inputFilePath + " has not been loaded");
        }
        
        return HResource();
    }

    Vector<SubResourceRaw> Importer::_importAll(const String& inputFilePath, SPtr<const ImportOptions> importOptions)
    {
        BaseImporter* importer = PrepareForImport(inputFilePath, importOptions);
        if (!importer)
            return Vector<SubResourceRaw>();

        Vector<SubResourceRaw> output = importer->ImportAll(inputFilePath, importOptions);
        return output;
    }

    SPtr<MultiResource> Importer::ImportAll(const String& inputFilePath, SPtr<const ImportOptions> importOptions)
    {
        Vector<SubResource> output;

        Vector<SubResourceRaw> importedResource = _importAll(inputFilePath, importOptions);
        for (auto& entry : importedResource)
        {
            if (entry.Res)
            {
                TE_DEBUG("Resource " + inputFilePath + " has been successfully loaded");

                HResource handle = gResourceManager()._createResourceHandle(entry.Res);
                output.push_back({ entry.Name, handle });
            }
            else
            {
                TE_DEBUG("Resource " + inputFilePath + "::" + entry.Name + " has not been loaded");
            }
        }

        return te_shared_ptr_new<MultiResource>(output);
    }

    bool Importer::SupportsFileType(const String& extension) const
    {
        for (const auto& assetImporter : _assetImporters)
        {
            if (assetImporter && assetImporter->IsExtensionSupported(extension))
                return true;
        }

        return false;
    }

    void Importer::RegisterAssetImporter(BaseImporter* importer)
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

        for (const auto& assetImporter : _assetImporters)
        {
            if (assetImporter && assetImporter->IsExtensionSupported(ext))
                return assetImporter;
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
            if (importOptions->GetCoreType() != defaultImportOptions->GetCoreType())
            {
                TE_ASSERT_ERROR(false, "Provided import options is not of valid type. Expected: " + 
                    ToString(defaultImportOptions->GetCoreType()) + ". Got: " + ToString(importOptions->GetCoreType()));
            }
        }

        return importer;
    }

    TE_CORE_EXPORT Importer& gImporter()
    {
        return Importer::Instance();
    }
}
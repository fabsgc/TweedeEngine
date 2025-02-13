#include "Importer/TeImporter.h"
#include "Importer/TeImportOptions.h"
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

    SPtr<Resource> Importer::_import(const std::filesystem::path& filePath, const ImportOptions& importOptions)
    {
        BaseImporter* importer = PrepareForImport(filePath);
        if (!importer)
            return nullptr;

        SPtr<Resource> output = importer->Import(filePath, importOptions);
        return output;
    }

    HResource Importer::Import(const std::filesystem::path& filePath, const ImportOptions& importOptions, const UUID& uuid)
    {
        SPtr<Resource> importedResource = _import(filePath, importOptions);

        if (importedResource)
        {
            TE_DEBUG("Resource from " + filePath.generic_string() + " has been successfully loaded");
            return gResourceManager()._createResourceHandle(importedResource, uuid);
        }
        else
        {
            TE_DEBUG("Resource from " + filePath.generic_string() + " has not been loaded");
        }
        
        return HResource();
    }

    Vector<SubResourceRaw> Importer::_importAll(const std::filesystem::path& filePath, const ImportOptions& importOptions)
    {
        BaseImporter* importer = PrepareForImport(filePath);
        if (!importer)
            return Vector<SubResourceRaw>();

        Vector<SubResourceRaw> output = importer->ImportAll(filePath, importOptions);
        return output;
    }

    SPtr<MultiResource> Importer::ImportAll(const std::filesystem::path& filePath, const ImportOptions& importOptions)
    {
        Vector<SubResource> output;

        Vector<SubResourceRaw> importedResource = _importAll(filePath, importOptions);
        for (auto& entry : importedResource)
        {
            if (entry.Res)
            {
                TE_DEBUG("Resource " + filePath.generic_string() + " has been successfully loaded");

                HResource handle = gResourceManager()._createResourceHandle(entry.Res);
                output.push_back({ entry.Name, handle });
            }
            else
            {
                TE_DEBUG("Resource " + filePath.generic_string() + "::" + entry.Name + " has not been loaded");
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

    BaseImporter* Importer::GetImporterForFile(const std::filesystem::path& filePath) const
    {
        String ext = Util::GetFileExtension(filePath.generic_string());

        if (ext.empty())
            return nullptr;

        ext = ext.substr(1, ext.size() - 1); // Remove the .
        if (!SupportsFileType(ext))
        {
            TE_ASSERT_ERROR(false, "There is no importer for the provided file : " + filePath.generic_string());
            return nullptr;
        }

        for (const auto& assetImporter : _assetImporters)
        {
            if (assetImporter && assetImporter->IsExtensionSupported(ext))
                return assetImporter;
        }

        return nullptr;
    }

    BaseImporter* Importer::PrepareForImport(const std::filesystem::path& filePath) const
    {
        return GetImporterForFile(filePath);
    }

    TE_CORE_EXPORT Importer& gImporter()
    {
        return Importer::Instance();
    }
}
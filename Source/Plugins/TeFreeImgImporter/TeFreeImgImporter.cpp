#include "TeFreeImgImporter.h"
#include "Importer/TeTextureImportOptions.h"
#include "FreeImage.h"

namespace te
{
    void FreeImageLoadErrorHandler(FREE_IMAGE_FORMAT fif, const char* message)
    {
        // Callback method as required by FreeImage to report problems
        const char* typeName = FreeImage_GetFormatFromFIF(fif);
        if (typeName)
        {
            TE_DEBUG("FreeImage error: " + String(message) + " when loading format " + String(typeName), __FILE__, __LINE__);
        }
        else
        {
            TE_DEBUG("FreeImage error: " + String(message), __FILE__, __LINE__);
        }
    }

    FreeImgImporter::FreeImgImporter()
    {
        _extensions.push_back(u8"jpeg");
        _extensions.push_back(u8"png");

        FreeImage_Initialise(false);

        // Set error handler
        FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);
    }

    FreeImgImporter::~FreeImgImporter()
    {
        //FreeImage_DeInitialise();
    }

    bool FreeImgImporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), tolower);

        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ImportOptions> FreeImgImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<TextureImportOptions>();
    }

    SPtr<Resource> FreeImgImporter::Import(const String& filePath, SPtr<const ImportOptions> importOptions)
    {
        return SPtr<Resource>();
    }
}
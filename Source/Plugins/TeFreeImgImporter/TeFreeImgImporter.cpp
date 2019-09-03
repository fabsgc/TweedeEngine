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

        // Register codecs
        StringStream strExt;
        strExt << "Supported formats: ";
        bool first = true;
        for (int i = 0; i < FreeImage_GetFIFCount(); ++i)
        {
            // Skip DDS codec since FreeImage does not have the option
            // to keep DXT data compressed, we'll use our own codec
            if ((FREE_IMAGE_FORMAT)i == FIF_DDS)
                continue;

            String exts = String(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));
            if (!first)
                strExt << ",";

            first = false;
            strExt << exts;

            // Pull off individual formats (separated by comma by FI)
            Vector<String> extsVector = Util::Split(exts, u8",");
            for (auto v = extsVector.begin(); v != extsVector.end(); ++v)
            {
                auto findIter = std::find(_extensions.begin(), _extensions.end(), *v);

                if (findIter == _extensions.end())
                {
                    String ext = *v;
                    Util::ToLowerCase(ext);

                    _extensions.push_back(ext);
                }
            }
        }

        // Set error handler
        FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);
    }

    FreeImgImporter::~FreeImgImporter()
    {
        FreeImage_DeInitialise();
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

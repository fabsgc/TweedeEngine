#include "TeFreeImgExporter.h"
#include "Exporter/TeTextureExportOptions.h"
#include "Image/TeColor.h"
#include "Image/TeTexture.h"
#include "Image/TePixelData.h"
#include "Image/TePixelUtil.h"
#include "Utility/TeBitwise.h"
#include "Utility/TeDataStream.h"
#include "Utility/TeFileSystem.h"

#include <cctype>
#include <filesystem>

namespace te
{
    void FreeImageLoadErrorHandler(FREE_IMAGE_FORMAT fif, const char* message)
    {
        // Callback method as required by FreeImage to report problems
        const char* typeName = FreeImage_GetFormatFromFIF(fif);
        if (typeName)
        {
            TE_DEBUG("FreeImage error: " + String(message) + " when exporting format " + String(typeName));
        }
        else
        {
            TE_DEBUG("FreeImage error: " + String(message));
        }
    }

    FreeImgExporter::FreeImgExporter()
        : BaseExporter()
    {
        FreeImage_Initialise(false);

        // Register codecs
        StringStream strExt;
        strExt << "Supported formats: ";
        bool first = true;
        for (int i = 0; i < FreeImage_GetFIFCount(); ++i)
        {
            // Skip DDS codec since FreeImage does not have the option
            // to keep DXT data compressed, we'll use our own codec
            //if ((FREE_IMAGE_FORMAT)i == FIF_DDS)
            //    continue;

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

                    _extensionToFID.insert(std::make_pair(ext, i));
                    _extensions.push_back(ext);
                }
            }
        }

        // Set error handler
        FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);
    }

    FreeImgExporter::~FreeImgExporter()
    {
        FreeImage_DeInitialise();
    }

    bool FreeImgExporter::IsExtensionSupported(const String& ext) const
    {
        String lowerCaseExt = ext;
        std::transform(lowerCaseExt.begin(), lowerCaseExt.end(), lowerCaseExt.begin(), 
            [](unsigned char c) -> unsigned char { return static_cast<unsigned char>(std::tolower(c)); });
        return find(_extensions.begin(), _extensions.end(), lowerCaseExt) != _extensions.end();
    }

    SPtr<ExportOptions> FreeImgExporter::CreateExportOptions() const
    {
        return te_shared_ptr_new<TextureExportOptions>();
    }

    bool FreeImgExporter::Export(const String& filePath, SPtr<const ExportOptions> exportOptions, bool force)
    {
        const TextureExportOptions* textureExportOptions = static_cast<const TextureExportOptions*>(exportOptions.get());

        return true;
    }
}

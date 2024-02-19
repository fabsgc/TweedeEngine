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

    bool FreeImgExporter::Export(void* object, const String& filePath, SPtr<const ExportOptions> exportOptions, bool force)
    {
        const TextureExportOptions* textureExportOptions = static_cast<const TextureExportOptions*>(exportOptions.get());

        Texture* texture = static_cast<Texture*>(object);
        if (!texture)
            return false;

        if (PixelUtil::IsCompressed(texture->GetProperties().GetFormat()))
        {
            TE_DEBUG("Texture is compressed and can't be exported : " + filePath);
            return false;
        }

        if (texture->GetProperties().GetTextureType() == TextureType::TEX_TYPE_3D)
        {
            TE_DEBUG("3D Texture are not handled yet : " + filePath);
            return false;
        }

        SPtr<PixelData> pixelData = texture->GetProperties().AllocBuffer(0, 0);
        texture->ReadData(*pixelData);

        return Export(*pixelData.get(), texture->GetProperties().GetWidth(), texture->GetProperties().GetHeight(), texture->GetProperties().IsHardwareGammaEnabled(), filePath, textureExportOptions, force);
    }

    bool FreeImgExporter::Export(const PixelData& pixelData,  UINT32 width, UINT32 height, bool isSRGB, const String& filePath, const TextureExportOptions* exportOptions, bool force)
    {
        int bitsDepth[4] = { 0, 0, 0, 0 };
        UINT32 bitsMask[4]  = { 0, 0, 0, 0 };

        PixelUtil::GetBitDepths(pixelData.GetFormat(), bitsDepth);
        PixelUtil::GetBitMasks(pixelData.GetFormat(), bitsMask);

        FIBITMAP* bitmap = FreeImage_Allocate(width, height, 32);
        RGBQUAD bitmapColor;

        if (!bitmap)
        {
            TE_DEBUG("Can't create bitmap object : " + filePath);
            return false;
        }

        for (UINT32 i = 0; i < width; i++)
        {
            for (UINT32 j = 0; j < height; j++)
            {
                Color pixelColor = pixelData.GetColorAt(i, j);

                if (isSRGB)
                {
                    pixelColor = pixelColor.GetLinear();
                }

                // TODO : Manage single channel texture (avoid red textures)
                // TODO : For Depth texture, allow to fit black and white values to the the range [0,1]
                // TODO : Separate Depth and Stencil buffer ?

                bitmapColor.rgbRed = static_cast<UINT8>(Math::Clamp01(pixelColor.r) * 255.0f);
                bitmapColor.rgbGreen = static_cast<UINT8>(Math::Clamp01(pixelColor.g) * 255.0f);
                bitmapColor.rgbBlue = static_cast<UINT8>(Math::Clamp01(pixelColor.b) * 255.0f);

                FreeImage_SetPixelColor(bitmap, i, height - 1 - j, &bitmapColor);
            }
        }

        if (!FreeImage_Save(FIF_PNG, bitmap, filePath.c_str()), PNG_Z_BEST_SPEED)
        {
            TE_DEBUG("Can't save bitmap object to file : " + filePath);
            FreeImage_Unload(bitmap);
            return false;
        }

        return true;
    }
}

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

    template<typename Predicate>
    void LoopThroughPixelData(const PixelData& pixelData, UINT32 width, UINT32 height, bool isSRGB, Predicate pred)
    {
        for (UINT32 i = 0; i < width; i++)
        {
            for (UINT32 j = 0; j < height; j++)
            {
                Color pixelColor = pixelData.GetColorAt(i, j);

                if (isSRGB)
                {
                    pixelColor = pixelColor.GetLinear();
                }

                pred(pixelColor, i, j);
            }
        }
    }

    bool FreeImgExporter::Export(const PixelData& pixelData, UINT32 width, UINT32 height, bool isSRGB, const String& filePath, const TextureExportOptions* exportOptions, bool force)
    {
        auto path = std::filesystem::absolute(filePath);

        if (std::filesystem::exists(path) && !force)
            return false;

        String extension = path.extension().generic_string();
        extension = extension.substr(1, extension.size() - 1); // Remove the .

        auto FID = _extensionToFID.find(extension);
        FREE_IMAGE_FORMAT FIFFormat = FREE_IMAGE_FORMAT::FIF_UNKNOWN;

        if (FID != _extensionToFID.end())
            FIFFormat = static_cast<FREE_IMAGE_FORMAT>(FID->second);
        else
            return false;

        FIBITMAP* bitmap = FreeImage_Allocate(width, height, 24);
        RGBQUAD bitmapColor;

        if (!bitmap)
        {
            TE_DEBUG("Can't create bitmap object : " + filePath);
            return false;
        }

        FreeImage_SetTransparent(bitmap, false);

        float minValue = std::numeric_limits<float>::max();
        float maxValue = std::numeric_limits<float>::min();

        if (exportOptions->IsDepthStencilBuffer)
        {
            LoopThroughPixelData(pixelData, width, height, isSRGB, [&](const Color& color, UINT32 i, UINT32 j) {
                minValue = std::min(minValue, color.r);
                maxValue = std::max(maxValue, color.r);
                });
        }

        LoopThroughPixelData(pixelData, width, height, isSRGB, [&](const Color& color, UINT32 i, UINT32 j) {
            // TODO : Separate Depth and Stencil buffer ?

            if (exportOptions->IsSingleChannel || exportOptions->IsDepthStencilBuffer)
            {
                float remappedColor = Math::Clamp01(color.r);
                if (exportOptions->IsDepthStencilBuffer)
                {
                    remappedColor = Math::Remap(Math::Clamp01(color.r), minValue, maxValue, 0.0f, 1.0f);
                }

                bitmapColor.rgbRed = static_cast<UINT8>(remappedColor * 255.0f);
                bitmapColor.rgbGreen = static_cast<UINT8>(remappedColor * 255.0f);
                bitmapColor.rgbBlue = static_cast<UINT8>(remappedColor * 255.0f);
            }
            else
            {
                bitmapColor.rgbRed = static_cast<UINT8>(Math::Clamp01(color.r) * 255.0f);
                bitmapColor.rgbGreen = static_cast<UINT8>(Math::Clamp01(color.g) * 255.0f);
                bitmapColor.rgbBlue = static_cast<UINT8>(Math::Clamp01(color.b) * 255.0f);
            }

            if (!FreeImage_SetPixelColor(bitmap, i, height - 1 - j, &bitmapColor))
            {
                TE_DEBUG("Can't save bitmap pixel color : ");
            }
            });

        if (!FreeImage_Save(FIFFormat, bitmap, filePath.c_str(), FIFFormat == FREE_IMAGE_FORMAT::FIF_JPEG ? JPEG_QUALITYSUPERB : 0))
        {
            TE_DEBUG("Can't save bitmap object to file : " + filePath);
            FreeImage_Unload(bitmap);
            return false;
        }

        FreeImage_Unload(bitmap);
        return true;
    }
}

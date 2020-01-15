#include "TeFreeImgImporter.h"
#include "Importer/TeTextureImportOptions.h"
#include "FreeImage.h"
#include "Image/TeTexture.h"
#include "Image/TePixelData.h"
#include "Utility/TeBitwise.h"
#include "Image/TePixelUtil.h"
#include "Utility/TeFileStream.h"

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

    String FreeImgImporter::MagicNumToExtension(const UINT8* magic, UINT32 maxBytes) const
    {
        // Set error handler
        FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);

        FIMEMORY* fiMem =
            FreeImage_OpenMemory((BYTE*)magic, static_cast<DWORD>(maxBytes));

        FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(fiMem, (int)maxBytes);
        FreeImage_CloseMemory(fiMem);

        if (fif != FIF_UNKNOWN)
        {
            String ext = String(FreeImage_GetFormatFromFIF(fif));
            Util::ToLowerCase(ext);
            return ext;
        }
        else
        {
            return "";
        }
    }

    SPtr<ImportOptions> FreeImgImporter::CreateImportOptions() const
    {
        return te_shared_ptr_new<TextureImportOptions>();
    }

    SPtr<Resource> FreeImgImporter::Import(const String& filePath, const SPtr<const ImportOptions> importOptions)
    {
        const TextureImportOptions* textureImportOptions = static_cast<const TextureImportOptions*>(importOptions.get());

        SPtr<PixelData> imgData = ImportRawImage(filePath);
        if (imgData == nullptr || imgData->GetData() == nullptr)
        {
            return nullptr;
        }

        Vector<SPtr<PixelData>> faceData;

        TextureType texType;
        if (textureImportOptions->IsCubemap)
        {
            TE_ASSERT_ERROR(false, "CubeMap is not supported yet", __FILE__, __LINE__);
            return nullptr;

            /*texType = TEX_TYPE_CUBE_MAP;

            std::array<SPtr<PixelData>, 6> cubemapFaces;
            if (GenerateCubemap(imgData, textureImportOptions->CubemapType, cubemapFaces))
            {
                faceData.insert(faceData.begin(), cubemapFaces.begin(), cubemapFaces.end());
            }
            else // Fall-back to 2D texture
            {
                texType = TEX_TYPE_2D;
                faceData.push_back(imgData);
            }*/

            // TODO
        }
        else
        {
            texType = TEX_TYPE_2D;
            faceData.push_back(imgData);
        }

        UINT32 numMips = 0;
        if (textureImportOptions->GenerateMips &&
            Bitwise::IsPow2(faceData[0]->GetWidth()) && Bitwise::IsPow2(faceData[0]->GetHeight()))
        {
            TE_ASSERT_ERROR(false, "Mipmap is not supported yet", __FILE__, __LINE__);
            return nullptr;

            /*UINT32 maxPossibleMip = PixelUtil::GetMaxMipmaps(faceData[0]->GetWidth(), faceData[0]->GetHeight(),
                faceData[0]->GetDepth(), faceData[0]->GetFormat());

            if (textureImportOptions->MaxMip == 0)
            {
                numMips = maxPossibleMip;
            }
            else
            {
                numMips = std::min(maxPossibleMip, textureImportOptions->MaxMip);
            }*/

            // TODO
        }

        int usage = TU_DEFAULT;
        if (textureImportOptions->CpuCached)
            usage |= TU_CPUCACHED;

        bool sRGB = textureImportOptions->SRGB;

        TEXTURE_DESC texDesc;
        texDesc.Type = texType;
        texDesc.Width = faceData[0]->GetWidth();
        texDesc.Height = faceData[0]->GetHeight();
        texDesc.NumMips = numMips;
        texDesc.Format = textureImportOptions->Format;
        texDesc.Usage = usage;
        texDesc.HwGamma = sRGB;

        SPtr<Texture> texture = Texture::_createPtr(texDesc);

        UINT32 numFaces = (UINT32)faceData.size();
        for (UINT32 i = 0; i < numFaces; i++)
        {
            Vector<SPtr<PixelData>> mipLevels;
            if (numMips > 0)
            {
                /*MipMapGenOptions mipOptions;
                mipOptions.isSRGB = sRGB;

                mipLevels = PixelUtil::GenMipmaps(*faceData[i], mipOptions);*/

                // TODO
            }
            else
            {
                mipLevels.push_back(faceData[i]);
            }

            for (UINT32 mip = 0; mip < (UINT32)mipLevels.size(); ++mip)
            {
                SPtr<PixelData> dst = texture->GetProperties().AllocBuffer(0, mip);

                PixelUtil::BulkPixelConversion(*mipLevels[mip], *dst);
                texture->WriteData(*dst, i, mip);
            }
        }

        texture->SetName(filePath);
        return texture;
    }

    SPtr<PixelData> FreeImgImporter::ImportRawImage(const String& filePath)
    {
        FileStream file(filePath);
        FREE_IMAGE_FORMAT imageFormat;

        if (file.Fail())
        {
            TE_ASSERT_ERROR(false, "Cannot open file: " + filePath, __FILE__, __LINE__);
            return nullptr;
        }

        size_t size = file.Size();

        if (size > std::numeric_limits<UINT32>::max())
        {
            TE_ASSERT_ERROR(false, "File size larger than supported!", __FILE__, __LINE__);
        }

        UINT32 magicLen = std::min((UINT32)size, 32u);
        UINT8 magicBuf[32];
        file.Read(static_cast<char*>((void*)magicBuf), static_cast<std::streamsize>(magicLen));
        file.Seek(0);

        String fileExtension = MagicNumToExtension(magicBuf, magicLen);
        auto findFormat = _extensionToFID.find(fileExtension);
        if (findFormat == _extensionToFID.end())
        {
            TE_ASSERT_ERROR(false, "Type of the file provided is not supported by this importer. File type: " + fileExtension, __FILE__, __LINE__);
        }

        imageFormat = (FREE_IMAGE_FORMAT)findFormat->second;
        
        uint8_t* data = static_cast<uint8_t*>(te_allocate(size));
        file.Read(static_cast<char*>((void*)data), static_cast<std::streamsize>(size));

        file.Close();

        FIMEMORY* fiMem = FreeImage_OpenMemory(data, static_cast<DWORD>(size));

        FIBITMAP* fiBitmap = FreeImage_LoadFromMemory((FREE_IMAGE_FORMAT)imageFormat, fiMem);
        if (!fiBitmap)
        {
            TE_ASSERT_ERROR(false, "Error decoding image", __FILE__, __LINE__);
        }

        UINT32 width = FreeImage_GetWidth(fiBitmap);
        UINT32 height = FreeImage_GetHeight(fiBitmap);
        PixelFormat format = PF_UNKNOWN;

        // Must derive format first, this may perform conversions

        FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(fiBitmap);
        FREE_IMAGE_COLOR_TYPE colourType = FreeImage_GetColorType(fiBitmap);
        unsigned bpp = FreeImage_GetBPP(fiBitmap);
        unsigned srcElemSize = 0;

        switch (imageType)
        {
        case FIT_UNKNOWN:
        case FIT_COMPLEX:
        case FIT_UINT32:
        case FIT_INT32:
        case FIT_DOUBLE:
        default:
            TE_ASSERT_ERROR(false, "Unknown or unsupported image format", __FILE__, __LINE__);

            break;
        case FIT_BITMAP:
            // Standard image type
            // Perform any colour conversions for greyscale
            if (colourType == FIC_MINISWHITE || colourType == FIC_MINISBLACK)
            {
                FIBITMAP* newBitmap = FreeImage_ConvertToGreyscale(fiBitmap);
                // free old bitmap and replace
                FreeImage_Unload(fiBitmap);
                fiBitmap = newBitmap;
                // get new formats
                bpp = FreeImage_GetBPP(fiBitmap);
                colourType = FreeImage_GetColorType(fiBitmap);
            }
            // Perform any colour conversions for RGB
            else if (bpp < 8 || colourType == FIC_PALETTE || colourType == FIC_CMYK)
            {
                FIBITMAP* newBitmap = FreeImage_ConvertTo24Bits(fiBitmap);
                // free old bitmap and replace
                FreeImage_Unload(fiBitmap);
                fiBitmap = newBitmap;
                // get new formats
                bpp = FreeImage_GetBPP(fiBitmap);
                colourType = FreeImage_GetColorType(fiBitmap);
            }

            // by this stage, 8-bit is greyscale, 16/24/32 bit are RGB[A]
            switch (bpp)
            {
            case 8:
                format = PF_R8;
                srcElemSize = 1;
                break;
            case 16:
                // Determine 555 or 565 from green mask
                // cannot be 16-bit greyscale since that's FIT_UINT16
                if (FreeImage_GetGreenMask(fiBitmap) == FI16_565_GREEN_MASK)
                {
                    assert(false && "Format not supported by the engine. TODO.");
                    return nullptr;
                }
                else
                {
                    assert(false && "Format not supported by the engine. TODO.");
                    return nullptr;
                    // FreeImage doesn't support 4444 format so must be 1555
                }
                srcElemSize = 2;
                break;
            case 24:
                // FreeImage differs per platform
                //     PF_BYTE_BGR[A] for little endian (== PF_ARGB native)
                //     PF_BYTE_RGB[A] for big endian (== PF_RGBA native)
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
                format = PF_RGB8;
#else
                format = PF_BGR8;
#endif
                srcElemSize = 3;
                break;
            case 32:
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
                format = PF_RGBA8;
#else
                format = PF_BGRA8;
#endif
                srcElemSize = 4;
                break;
            };
            break;
        case FIT_UINT16:
        case FIT_INT16:
            // 16-bit greyscale
            assert(false && "No INT pixel formats supported currently. TODO.");
            return nullptr;
            break;
        case FIT_FLOAT:
            // Single-component floating point data
            format = PF_R32F;
            srcElemSize = 4;
            break;
        case FIT_RGB16:
            format = PF_RGBA16;
            srcElemSize = 2 * 3;
            break;
        case FIT_RGBA16:
            format = PF_RGBA16;
            srcElemSize = 2 * 4;
            break;
        case FIT_RGBF:
            format = PF_RGB32F;
            srcElemSize = 4 * 3;
            break;
        case FIT_RGBAF:
            format = PF_RGBA32F;
            srcElemSize = 4 * 4;
            break;
        };

        unsigned char* srcData = FreeImage_GetBits(fiBitmap);
        unsigned srcPitch = FreeImage_GetPitch(fiBitmap);

        // Final data - invert image and trim pitch at the same time
        UINT32 dstElemSize = PixelUtil::GetNumElemBytes(format);
        UINT32 dstPitch = width * PixelUtil::GetNumElemBytes(format);

        // Bind output buffer
        SPtr<PixelData> texData = te_shared_ptr_new<PixelData>(width, height, 1, format);
        texData->AllocateInternalBuffer();
        UINT8* output = texData->GetData();

        UINT8* pSrc;
        UINT8* pDst = output;

        // Copy row by row, which is faster
        if (srcElemSize == dstElemSize)
        {
            for (UINT32 y = 0; y < height; ++y)
            {
                pSrc = srcData + (height - y - 1) * srcPitch;
                memcpy(pDst, pSrc, dstPitch);
                pDst += dstPitch;
            }
        }
        else
        {
            for (UINT32 y = 0; y < height; ++y)
            {
                pSrc = srcData + (height - y - 1) * srcPitch;

                for (UINT32 x = 0; x < width; ++x)
                    memcpy(pDst + x * dstElemSize, pSrc + x * srcElemSize, srcElemSize);

                pDst += dstPitch;
            }
        }

        FreeImage_Unload(fiBitmap);
        FreeImage_CloseMemory(fiMem);

        te_delete(data);

        return texData;
    }
}

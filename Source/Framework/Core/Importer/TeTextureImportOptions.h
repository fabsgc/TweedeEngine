#pragma once

#include "TeCorePrerequisites.h"
#include "Importer/TeImportOptions.h"
#include "Image/TePixelData.h"

namespace te
{
    /** Contains import options you may use to control how is a texture imported. */
    class TE_CORE_EXPORT TextureImportOptions : public ImportOptions
    {
    public:
        TextureImportOptions();

        /** Pixel format to import as. */
        PixelFormat Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;

        /** Enables or disables mipmap generation for the texture. */
        bool GenerateMips = false;

        /** Enable this to rescale the alpha values of computed mipmaps so coverage is preserved. 
         * This means a higher percentage of pixels passes the alpha test and lower mipmap levels do not become more transparent  
         */
        bool MipMapsPreserveCoverage = false;

        /** Maximum mip level to generate when generating mipmaps. If 0 then maximum amount of mip levels will be generated. */
        UINT32 MaxMip = 0;

        /**
         * Determines whether the texture data should be treated as if its in sRGB (gamma) space. Such texture will be
         * converted by hardware to linear space before use on the GPU.
         */
        bool SRGB = false;

        /** Determines whether the texture data is also stored in main memory, available for fast CPU access. */
        bool CpuCached = false;

        /**
         * Determines should the texture be imported as a cubemap. See setCubemapSource to choose how will the source
         * texture be converted to a cubemap.
         */
        bool IsCubemap = false;

        CubemapSourceType CubemapType = CubemapSourceType::Faces;

        /** Creates a new import options object that allows you to customize how are textures imported. */
        static SPtr<TextureImportOptions> Create();
    };
}

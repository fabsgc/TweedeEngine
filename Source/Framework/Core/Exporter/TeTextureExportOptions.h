#pragma once

#include "TeCorePrerequisites.h"
#include "Exporter/TeExportOptions.h"
#include "Image/TePixelData.h"

namespace te
{
    /** Contains import options you may use to control how is a texture imported. */
    class TE_CORE_EXPORT TextureExportOptions : public ExportOptions
    {
    public:
        TextureExportOptions();

        /** Pixel format to export as. */
        PixelFormat Format = Util::IsBigEndian() ? PF_RGBA8 : PF_BGRA8;

        /** Enables or disables mipmap generation for the texture. */
        bool ExportMips = false;

        /** Maximum mip level to generate when generating mipmaps. If 0 then maximum amount of mip levels will be generated. */
        UINT32 MaxMip = 0;

        /**
         * Determines should the texture be exported as a cubemap. See setCubemapSource to choose how will the source
         * texture be converted to a cubemap.
         */
        bool IsCubeMap = false;

        CubemapSourceType CubemapType = CubemapSourceType::Faces;

        /** Creates a new import options object that allows you to customize how are textures imported. */
        static SPtr<TextureExportOptions> Create();
    };
}

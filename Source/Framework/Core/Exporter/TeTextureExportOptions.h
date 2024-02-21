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

        /** Enables or disables mipmap generation for the texture. If false, biggest level will be exported */
        bool ExportMips = false;

        /** Maximum mip level to generate when generating mipmaps. If 0 then maximum amount of mip levels will be generated. */
        UINT32 MaxMip = 0;

        /**
         * Find minimal and maximal values for each channel and clamp them to [0,1] 
         * Write Stencil buffer to a second texture
         */
        bool IsDepthStencilBuffer = false;

        /** For some textures, only red channel is filled. To avoid them from being rendered completely red, you can set this to true */
        bool IsSingleChannel = false;

        /** Creates a new import options object that allows you to customize how are textures imported. */
        static SPtr<TextureExportOptions> Create();
    };
}

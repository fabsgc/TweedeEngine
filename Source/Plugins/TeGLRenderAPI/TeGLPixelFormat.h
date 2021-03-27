#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "Image/TePixelUtil.h"

namespace te
{
    /**	Converts pixel formats to OpenGL formats */
    class GLPixelUtil
    {
    public:
        /**	Finds the closest pixel format that OpenGL supports. */
        static PixelFormat GetClosestSupportedPF(PixelFormat format, TextureType texType, int usage);

        /**	Returns matching OpenGL base pixel format type if one is found, zero otherwise. */
        static GLenum GetGLOriginFormat(PixelFormat format);
    
        /**	Returns OpenGL data type used in the provided format. */
        static GLenum GetGLOriginDataType(PixelFormat format);
        
        /**
         * Returns matching OpenGL internal pixel format type if one is found, zero otherwise. Optionally returns an SRGB
         * format if @p HWGamma is specified and such format exists.
         */
        static GLenum GetGLInternalFormat(PixelFormat format, bool hwGamma = false);
    
        /** Returns an OpenGL type that should be used for creating a buffer for the specified depth/stencil format. */
        static GLenum GetDepthStencilTypeFromPF(PixelFormat format);

        /** Returns an OpenGL format that should be used for creating a buffer for the specified depth/stencil format. */
        static GLenum GetDepthStencilFormatFromPF(PixelFormat format);

        /** Converts engine GPU buffer format to OpenGL GPU buffer format. */
        static GLenum GetBufferFormat(GpuBufferFormat format);

    };
}
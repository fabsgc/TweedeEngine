#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeVideoMode.h"
#include "Utility/TeEvent.h"
#include "CoreUtility/TeCoreObject.h"
#include "Utility/TeNonCopyable.h"

namespace te
{
    struct TE_CORE_EXPORT RENDER_SURFACE_DESC
    {
        RENDER_SURFACE_DESC() { }

        SPtr<Texture> Tex;

        /** First face of the texture to bind (array index in texture arrays, or Z slice in 3D textures). */
        UINT32 Face = 0;

        /**
         * Number of faces to bind (entries in a texture array, or Z slices in 3D textures). When zero the entire resource
         * will be bound.
         */
        UINT32 NumFaces = 0;

        /** If the texture has multiple mips, which one to bind (only one can be bound for rendering). */
        UINT32 MipLevel = 0;
    };

    /** Contains various properties that describe a render target. */
    class TE_CORE_EXPORT RenderTargetProperties
    {
    public:
        virtual ~RenderTargetProperties() = default;

        /** Width of the render target, in pixels. */
        UINT32 Width = 0;

        /** Height of the render target, in pixels. */
        UINT32 Height = 0;

        /**
         * Number of three dimensional slices of the render target. This will be number of layers for array
         * textures or number of faces cube textures.
         */
        UINT32 NumSlices = 0;

        /**
         * Controls in what order is the render target rendered to compared to other render targets. Targets with higher
         * priority will be rendered before ones with lower priority.
         */
        INT32 Priority = 0;

        /**
         * True if the render target will wait for vertical sync before swapping buffers. This will eliminate
         * tearing but may increase input latency.
         */
        bool VSync = false;

        /** True if pixels written to the render target will be gamma corrected. */
        bool HWGamma = false;

        /**
         * Does the texture need to be vertically flipped because of different screen space coordinate systems.	(Determines
         * is origin top left or bottom left. Engine default is top left.)
         */
        bool RequiresTextureFlipping = false;

        /** True if the target is a window, false if an offscreen target. */
        bool IsWindow = false;

        /** Controls how many samples are used for multisampling. (0 or 1 if multisampling is not used). */
        UINT32 MultisampleCount = 0;
    };

    /**
     * Provides access to internal render target implementation
     */
    class TE_CORE_EXPORT RenderTarget : public CoreObject, public NonCopyable
    {
    public:
        /** Frame buffer type when double-buffering is used. */
        enum FrameBuffer
        {
            FB_FRONT,
            FB_BACK,
            FB_AUTO
        };

        RenderTarget() = default;
        virtual ~RenderTarget() = default;

        /**
         * Sets a priority that determines in which orders the render targets the processed.
         *
         * @param[in]	priority	The priority. Higher value means the target will be rendered sooner.
         */
        void SetPriority(INT32 priority);

        /** Event that gets triggered whenever the render target is resized. */
        mutable Event<void()> OnResized;

        /** Swaps the frame buffers to display the next frame. */
        virtual void SwapBuffers() {}

        /** Queries the render target for a custom attribute. This may be anything and is implementation specific. */
        virtual void GetCustomAttribute(const String& name, void* pData) const;

        /**	Returns properties that describe the render target. */
        virtual const RenderTargetProperties& GetProperties() const = 0;
    };
}

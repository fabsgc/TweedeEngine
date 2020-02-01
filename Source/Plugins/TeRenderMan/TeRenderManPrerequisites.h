#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

namespace te
{
    /** Available implementation of the RenderElement class. */
    enum class RenderElementType
    {
        /** See RenderableElement. */
        Renderable,
        /** See ParticlesRenderElement. */
        Particle,
        /** See DecalRenderElement. */
        Decal
    };

    /** Types of ways for shaders to handle MSAA. */
    enum class MSAAMode
    {
        /** No MSAA supported. */
        None,
        /** Single MSAA sample will be resolved. */
        Single,
        /** All MSAA samples will be resolved. */
        Full,
    };

    /** State used to controlling how are properties that need to maintain their previous frame state updated. */
    enum class PrevFrameDirtyState
    {
        /** Most recent version of the property was updated this frame, and its old data stored as prev. version. */
        Updated,
        /** No update has been done this frame, most recent version of the properties should be copied into prev. frame. */
        CopyMostRecent,
        /** Most recent and prev. frame versions are the same and require no updates. */
        Clean
    };

    /** Information about current time and frame index. */
    struct FrameTimings
    {
        float Time = 0.0f;
        float TimeDelta = 0.0f;
        UINT64 FrameIdx = 0;
    };

    struct RenderManOptions;
    class RenderMan;
    class RendererScene;
    class RenderView;
    struct LightData;
    class RendererLight;
    class RenderableElement;
    struct RendererRenderable;
}

#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeVector3.h"
#include "RenderAPI/TeSubMesh.h"

namespace te 
{
    /** Contains data needed for performing a single rendering pass. */
    struct RenderQueueElement
    { };

    /**
     * Render objects determines rendering order of objects contained within it. Rendering order is determined by object
     * material, and can influence rendering of transparent or opaque objects, or be used to improve performance by grouping
     * similar objects together.
     */
    class TE_CORE_EXPORT RenderQueue
    {
    public:
        RenderQueue() = default;
        ~RenderQueue() = default;

        void Add() { }
        void Sort() { }
        void Clear() { }
    };
}
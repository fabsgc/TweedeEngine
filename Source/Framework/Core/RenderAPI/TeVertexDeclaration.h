#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /**	Describes a single vertex element in a vertex declaration. */
    class TE_CORE_EXPORT VertexElement
    {
    public:
        VertexElement() = default;
        ~VertexElement() = default;
    };

    /**
     * Describes a set of vertex elements, used for describing contents of a vertex buffer or inputs to a vertex GPU program.
     */
    class TE_CORE_EXPORT VertexDeclaration : public CoreObject
    {
    public:
        virtual ~VertexDeclaration() { }

    protected:
        friend class HardwareBufferManager;
    };
}

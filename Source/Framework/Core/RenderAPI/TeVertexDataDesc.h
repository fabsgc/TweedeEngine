#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
	/**
	 * Contains information about layout of vertices in a buffer. This is very similar to VertexDeclaration but unlike
	 * VertexDeclaration it has no render API object to back it up and is very lightweight.
	 */
	class TE_CORE_EXPORT VertexDataDesc
	{
	public:
		VertexDataDesc() = default;
        ~VertexDataDesc() = default;
    };
}

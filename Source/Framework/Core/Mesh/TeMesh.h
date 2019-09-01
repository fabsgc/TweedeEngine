#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
	class TE_CORE_EXPORT Mesh : public Resource
	{
	public:
		Mesh() = default;
		virtual ~Mesh() = default;
    };
}
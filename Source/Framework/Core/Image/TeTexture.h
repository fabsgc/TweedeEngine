#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
	class TE_CORE_EXPORT Texture : public Resource
	{
	public:
		Texture() = default;
		virtual ~Texture() = default;
    };
}
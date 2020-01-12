#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
	class TE_CORE_EXPORT Shader : public Resource
	{
	public:
		Shader() = default;
		virtual ~Shader() = default;
    };
}

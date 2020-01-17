#pragma once

#include "Prerequisites/TePrerequisitesUtility.h"

namespace te
{
	/** Serializable blob of raw memory. */
	struct DataBlob
	{
		UINT8* Data = nullptr;
		UINT32 Size = 0;
	};
}

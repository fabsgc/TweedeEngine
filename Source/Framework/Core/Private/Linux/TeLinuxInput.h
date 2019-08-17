#pragma once

#include "TeCorePrerequisites.h"
#include "Input/TeInputData.h"

namespace te
{
    /** Infomation about an analog axis that's part of a gamepad. */
	struct AxisInfo
	{
		INT32 AxisIdx;
		INT32 Min;
		INT32 Max;
	};

	/** Information about a gamepad. */
	struct GamepadInfo
	{
		UINT32 Id;
		UINT32 EventHandlerIdx;
		String Name;

		UnorderedMap<INT32, ButtonCode> ButtonMap;
		UnorderedMap<INT32, AxisInfo> AxisMap;
	};

	/**
	 * Data specific to Linux implementation of the input system. Can be passed to platform specific implementations of
	 * the individual device types.
	 */
	struct InputPrivateData
	{
		Vector<GamepadInfo> GamepadInfos;
	};
}
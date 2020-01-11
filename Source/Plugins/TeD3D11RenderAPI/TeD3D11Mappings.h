#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeCommonTypes.h"

namespace te
{
    /**	Helper class that maps engine types to DirectX 11 types. */
	class D3D11Mappings
	{
	public:
        /**	Converts engine to DX11 specific comparison function. */
		static D3D11_COMPARISON_FUNC Get(CompareFunction cf);

		/**	Converts engine to DX11 specific culling mode. */
		static D3D11_CULL_MODE Get(CullingMode cm);

		/**	Converts engine to DX11 specific polygon fill mode. */
		static D3D11_FILL_MODE Get(PolygonMode mode);

        /**	Returns DX11 primitive topology based on the provided draw operation type. */
		static D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType(DrawOperationType type);

		/**	Converts engine color to DX11 color. */
		static void Get(const Color& inColor, float* outColor);
    };
}

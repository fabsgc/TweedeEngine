#include "TeD3D11Mappings.h"
#include "Utility/TeColor.h"

namespace te
{
    D3D11_COMPARISON_FUNC D3D11Mappings::Get(CompareFunction cf)
	{
		switch(cf)
		{
		case CMPF_ALWAYS_FAIL:
			return D3D11_COMPARISON_NEVER;
		case CMPF_ALWAYS_PASS:
			return D3D11_COMPARISON_ALWAYS;
		case CMPF_LESS:
			return D3D11_COMPARISON_LESS;
		case CMPF_LESS_EQUAL:
			return D3D11_COMPARISON_LESS_EQUAL;
		case CMPF_EQUAL:
			return D3D11_COMPARISON_EQUAL;
		case CMPF_NOT_EQUAL:
			return D3D11_COMPARISON_NOT_EQUAL;
		case CMPF_GREATER_EQUAL:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case CMPF_GREATER:
			return D3D11_COMPARISON_GREATER;
		};

		// Unsupported type
		return D3D11_COMPARISON_ALWAYS;
	}

    D3D11_CULL_MODE D3D11Mappings::Get(CullingMode cm)
	{
		switch(cm)
		{
		case CULL_NONE:
			return D3D11_CULL_NONE;
		case CULL_CLOCKWISE:
			return D3D11_CULL_FRONT;
		case CULL_COUNTERCLOCKWISE:
			return D3D11_CULL_BACK;
		}

		// Unsupported type
		return D3D11_CULL_NONE;
	}

	D3D11_FILL_MODE D3D11Mappings::Get(PolygonMode mode)
	{
		switch(mode)
		{
		case PM_WIREFRAME:
			return D3D11_FILL_WIREFRAME;
		case PM_SOLID:
			return D3D11_FILL_SOLID;
		}

		return D3D11_FILL_SOLID;
	}

    D3D11_PRIMITIVE_TOPOLOGY D3D11Mappings::GetPrimitiveType(DrawOperationType type)
	{
		switch(type)
		{
		case DOT_POINT_LIST:
			return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case DOT_LINE_LIST:
			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case DOT_LINE_STRIP:
			return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case DOT_TRIANGLE_LIST:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case DOT_TRIANGLE_STRIP:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case DOT_TRIANGLE_FAN:
			TE_ASSERT_ERROR(false, "D3D11 doesn't support triangle fan primitive type.", __FILE__, __LINE__);
		}

		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}

    void D3D11Mappings::Get(const Color& inColor, float* outColor)
	{
		outColor[0] = inColor.r;
		outColor[1] = inColor.g;
		outColor[2] = inColor.b;
		outColor[3] = inColor.a;	
	}
}

#include "TeD3D11Mappings.h"
#include "Utility/TeColor.h"

namespace te
{
	D3D11_BLEND D3D11Mappings::Get(BlendFactor bf)
	{
		switch(bf)
		{
		case BF_ONE:
			return D3D11_BLEND_ONE;
		case BF_ZERO:
			return D3D11_BLEND_ZERO;
		case BF_DEST_COLOR:
			return D3D11_BLEND_DEST_COLOR;
		case BF_SOURCE_COLOR:
			return D3D11_BLEND_SRC_COLOR;
		case BF_INV_DEST_COLOR:
			return D3D11_BLEND_INV_DEST_COLOR;
		case BF_INV_SOURCE_COLOR:
			return D3D11_BLEND_INV_SRC_COLOR;
		case BF_DEST_ALPHA:
			return D3D11_BLEND_DEST_ALPHA;
		case BF_SOURCE_ALPHA:
			return D3D11_BLEND_SRC_ALPHA;
		case BF_INV_DEST_ALPHA:
			return D3D11_BLEND_INV_DEST_ALPHA;
		case BF_INV_SOURCE_ALPHA:
			return D3D11_BLEND_INV_SRC_ALPHA;
		}

		// Unsupported type
		return D3D11_BLEND_ZERO;
	}

	D3D11_BLEND_OP D3D11Mappings::Get(BlendOperation bo)
	{
		switch(bo)
		{
		case BO_ADD:
			return D3D11_BLEND_OP_ADD;
		case BO_SUBTRACT:
			return D3D11_BLEND_OP_SUBTRACT;
		case BO_REVERSE_SUBTRACT:
			return D3D11_BLEND_OP_REV_SUBTRACT;
		case BO_MIN:
			return D3D11_BLEND_OP_MIN;
		case BO_MAX:
			return D3D11_BLEND_OP_MAX;
		}

		// Unsupported type
		return D3D11_BLEND_OP_ADD;
	}

    D3D11_STENCIL_OP D3D11Mappings::Get(StencilOperation op, bool invert)
    {
        switch (op)
        {
        case SOP_KEEP:
            return D3D11_STENCIL_OP_KEEP;
        case SOP_ZERO:
            return D3D11_STENCIL_OP_ZERO;
        case SOP_REPLACE:
            return D3D11_STENCIL_OP_REPLACE;
        case SOP_INCREMENT:
            return invert ? D3D11_STENCIL_OP_DECR_SAT : D3D11_STENCIL_OP_INCR_SAT;
        case SOP_DECREMENT:
            return invert ? D3D11_STENCIL_OP_INCR_SAT : D3D11_STENCIL_OP_DECR_SAT;
        case SOP_INCREMENT_WRAP:
            return invert ? D3D11_STENCIL_OP_DECR : D3D11_STENCIL_OP_INCR;
        case SOP_DECREMENT_WRAP:
            return invert ? D3D11_STENCIL_OP_INCR : D3D11_STENCIL_OP_DECR;
        case SOP_INVERT:
            return D3D11_STENCIL_OP_INVERT;
        }

        // Unsupported type
        return D3D11_STENCIL_OP_KEEP;
    }

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

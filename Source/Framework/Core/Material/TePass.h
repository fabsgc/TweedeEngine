#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"
#include "Image/TeColor.h"
#include "RenderAPI/TeBlendState.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeGpuProgram.h"
#include "RenderAPI/TeGpuPipelineState.h"

namespace te
{
    /** Descriptor structure used for initializing a shader pass. */
	struct PASS_DESC
	{
		BLEND_STATE_DESC BlendStateDesc;
		RASTERIZER_STATE_DESC RasterizerStateDesc;
		DEPTH_STENCIL_STATE_DESC DepthStencilStateDesc;
		UINT32 StencilRefValue;

		GPU_PROGRAM_DESC VertexProgramDesc;
		GPU_PROGRAM_DESC PixelProgramDesc;
		GPU_PROGRAM_DESC GeometryProgramDesc;
		GPU_PROGRAM_DESC HullProgramDesc;
		GPU_PROGRAM_DESC DomainProgramDesc;
	};

    class TE_CORE_EXPORT Pass : public CoreObject
    {
    public:
        Pass() = default;
        ~Pass() = default;

    protected:
        PASS_DESC _data;
		SPtr<GraphicsPipelineState> _graphicsPipelineState;
    };
}

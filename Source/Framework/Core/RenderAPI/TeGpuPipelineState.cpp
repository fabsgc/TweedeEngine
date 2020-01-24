#include "TeGpuPipelineState.h"
#include "RenderAPI/TeRenderStateManager.h"
#include "RenderAPI/TeGpuProgram.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeBlendState.h"
#include "RenderAPI/TeGpuParamDesc.h"
#include "RenderAPI/TeGpuPipelineParamInfo.h"
#include "RenderAPI/TeRenderStateManager.h"

namespace te
{
    GraphicsPipelineState::GraphicsPipelineState(const PIPELINE_STATE_DESC& desc, GpuDeviceFlags deviceMask)
        : _data(desc)
        , _deviceMask(deviceMask)
    { }

    void GraphicsPipelineState::Initialize()
    {
        GPU_PIPELINE_PARAMS_DESC paramsDesc;

        if (_data.vertexProgram != nullptr)
            paramsDesc.VertexParams = _data.vertexProgram->GetParamDesc();
        if (_data.pixelProgram != nullptr)
            paramsDesc.PixelParams = _data.pixelProgram->GetParamDesc();
        if (_data.geometryProgram != nullptr)
            paramsDesc.GeometryParams = _data.geometryProgram->GetParamDesc();
        if (_data.hullProgram != nullptr)
            paramsDesc.HullParams = _data.hullProgram->GetParamDesc();
        if (_data.domainProgram != nullptr)
            paramsDesc.DomainParams = _data.domainProgram->GetParamDesc();

        _paramInfo = GpuPipelineParamInfo::Create(paramsDesc);

        CoreObject::Initialize();
    }

    SPtr<GraphicsPipelineState> GraphicsPipelineState::Create(const PIPELINE_STATE_DESC& desc, GpuDeviceFlags deviceMask)
    {
        return RenderStateManager::Instance().CreateGraphicsPipelineState(desc, deviceMask);
    }
}

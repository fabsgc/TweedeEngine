#include "TeGpuParams.h"
#include "RenderAPI/TeHardwareBufferManager.h"
#include "RenderAPI/TeGpuPipelineState.h"

namespace te
{
    const TextureSurface GpuParams::COMPLETE = TextureSurface(0, 0, 0, 0);

    GpuParams::GpuParams(const SPtr<GpuPipelineParamInfo>& paramInfo, GpuDeviceFlags deviceMask)
    { }

    SPtr<GpuParams> GpuParams::Create(const SPtr<GraphicsPipelineState>& pipelineState, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateGpuParams(pipelineState->GetParamInfo(), deviceMask);
    }

    SPtr<GpuParams> GpuParams::Create(const SPtr<GpuPipelineParamInfo>& paramInfo, GpuDeviceFlags deviceMask)
    {
        return HardwareBufferManager::Instance().CreateGpuParams(paramInfo, deviceMask);
    }

    SPtr<GpuParams> GpuParams::_getThisPtr() const
    {
        return std::static_pointer_cast<GpuParams>(GetThisPtr());
    }
}
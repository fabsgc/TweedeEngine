#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeGpuParam.h"
#include "CoreUtility/TeCoreObject.h"
#include "Math/TeMatrixNxM.h"

namespace te
{
    class TE_CORE_EXPORT GpuParams : public CoreObject
	{
	public:
        /** Surface that covers all texture sub-resources. */
        static const TextureSurface COMPLETE;

		virtual ~GpuParams() = default;

        /**
         * @copydoc bs::GpuParams::create(const SPtr<GraphicsPipelineState>&)
         * @param[in]	deviceMask		Mask that determines on which GPU devices should the buffer be created on.
         */
        static SPtr<GpuParams> Create(const SPtr<GraphicsPipelineState>& pipelineState,
            GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /**
         * @copydoc bs::GpuParams::create(const SPtr<GpuPipelineParamInfo>&)
         * @param[in]	deviceMask		Mask that determines on which GPU devices should the buffer be created on.
         */
        static SPtr<GpuParams> Create(const SPtr<GpuPipelineParamInfo>& paramInfo,
            GpuDeviceFlags deviceMask = GDF_DEFAULT);


        // TODO

    protected:
        friend class HardwareBufferManager;

        GpuParams(const SPtr<GpuPipelineParamInfo>& paramInfo, GpuDeviceFlags deviceMask);

        /** @copydoc CoreObject::GetThisPtr */
        SPtr<GpuParams> _getThisPtr() const;

    protected:
		SPtr<GpuPipelineParamInfo> _paramInfo;
    };
}
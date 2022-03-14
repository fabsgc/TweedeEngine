#include "TeRenderManIBLUtility.h"
#include "Image/TETexture.h"
#include "Renderer/TeRendererUtility.h"
#include "RenderAPI/TeGpuBuffer.h"
#include "TeRenderMan.h"

namespace te
{
    void RenderManIBLUtility::FilterCubemapForSpecular(const SPtr<Texture>& cubemap, const SPtr<Texture>& scratch) const
    {
        // TODO PBR
    }

    void RenderManIBLUtility::FilterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output) const
    {
        // TODO PBR
    }

    void RenderManIBLUtility::FilterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output,
        UINT32 outputIdx) const
    {
        // TODO PBR
    }

    void RenderManIBLUtility::ScaleCubemap(const SPtr<Texture>& src, UINT32 srcMip, const SPtr<Texture>& dst,
        UINT32 dstMip) const
    {
        // TODO PBR
    }

    void RenderManIBLUtility::DownsampleCubemap(const SPtr<Texture>& src, UINT32 srcMip, const SPtr<Texture>& dst,
        UINT32 dstMip)
    {
        // TODO PBR
    }

    void RenderManIBLUtility::FilterCubemapForIrradianceNonCompute(const SPtr<Texture>& cubemap, UINT32 outputIdx,
        const SPtr<RenderTexture>& output)
    {
        // TODO PBR
    }
}

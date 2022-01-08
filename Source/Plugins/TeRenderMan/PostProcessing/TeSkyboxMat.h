#pragma once

#include "TeCorePrerequisites.h"
#include "Renderer/TeRendererMaterial.h"
#include "Renderer/TeParamBlocks.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(SkyboxParamDef)
        TE_PARAM_BLOCK_ENTRY(Color, gClearColor)
        TE_PARAM_BLOCK_ENTRY(float, gBrightness)
        TE_PARAM_BLOCK_ENTRY(UINT32, gUseTexture)
    TE_PARAM_BLOCK_END

    extern SkyboxParamDef gSkyboxParamDef;

    /** 
     * Shader that renders a skybox using a cubemap or a solid color. 
     */
    class SkyboxMat : public RendererMaterial<SkyboxMat>
    {
        RMAT_DEF(BuiltinShader::Skybox);

    public:
        SkyboxMat();

        /** Binds the material for rendering and sets up any parameters. */
        void Bind(const SPtr<GpuParamBlockBuffer>& perCamera, const SPtr<Texture>& texture, const Color& solidColor, const float& brightness);

    private:
        SPtr<GpuParamBlockBuffer> _paramBuffer;
    };
}

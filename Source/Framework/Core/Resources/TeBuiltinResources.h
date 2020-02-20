#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

#include "RenderAPI/TeBlendState.h"
#include "RenderAPI/TeRasterizerState.h"
#include "RenderAPI/TeDepthStencilState.h"
#include "RenderAPI/TeSamplerState.h"

#include "Math/TeMath.h"
#include "Math/TeMatrix4.h"

#include "RenderAPI/TeGpuProgram.h"

#include "Material/TeShader.h"

namespace te
{
    /** Types of builtin shaders that are always available. */
    enum class BuiltinShader
    {
        /** Shader used for opaque 3D geometry. */
        Opaque,
        /** Shader used for transparent 3D geometry. */
        Transparent,
        /** Shader used to blit a texture on a screen quad */
        Blit,
        /** Shader used to render a skybox in a scene */
        Skybox
    };

    /** Types of builtin shaders that are always available. */
    enum class BuiltinSampler
    {
        /** Shader used for opaque 3D geometry. */
        Anisotropic,
        /** Shader used for transparent 3D geometry. */
        Bilinear
    };

    /**	Holds references to built-in resources used by the core engine. */
    class TE_CORE_EXPORT BuiltinResources : public Module<BuiltinResources>
    {
    public:
        BuiltinResources();
        ~BuiltinResources();

        /** @copydoc Module::OnStartUp */
        void OnStartUp() override;

        /** @copydoc Module::OnShutDown */
        void OnShutDown() override;

        TE_MODULE_STATIC_HEADER_MEMBER(BuiltinResources)

        /**	Returns one of the builtin shader types. */
        HShader GetBuiltinShader(BuiltinShader type);

        /**	Returns one of the builtin sampler types. */
        SPtr<SamplerState> GetBuiltinSampler(BuiltinSampler type);

    public:
        static constexpr const char* MATERIALS_FOLDER = "Data/Materials/";
        static constexpr const char* MESHES_FOLDER = "Data/Meshes/";
        static constexpr const char* SHADERS_FOLDER = "Data/Shaders/";
        static constexpr const char* TEXTURES_FOLDER = "Data/Textures/";

        struct PerInstanceData
        {
            Matrix4 gMatWorld;
            Matrix4 gMatInvWorld;
            Matrix4 gMatWorldNoScale;
            Matrix4 gMatInvWorldNoScale;
            Matrix4 gMatPrevWorld;
            UINT32  gLayer;
        };

    private:
        void InitGpuPrograms();
        void InitStates();
        void InitShaderDesc();

        void InitSamplers();

        void InitShaderOpaque();
        void InitShaderTransparent();
        void InitShaderBlit();
        void InitShaderSkybox();

    private:
        HShader _shaderOpaque;
        HShader _shaderTransparent;
        HShader _shaderBlit;
        HShader _shaderSkybox;

        SPtr<SamplerState> _anisotropicSamplerState = nullptr;
        SPtr<SamplerState> _bilinearSamplerState = nullptr;

        SHADER_DESC _forwardShaderDesc;
        SHADER_DESC _blitShaderDesc;
        SHADER_DESC _skyboxShaderDesc;

        GPU_PROGRAM_DESC _vertexShaderForwardDesc;
        GPU_PROGRAM_DESC _pixelShaderForwardDesc;

        GPU_PROGRAM_DESC _vertexShaderBlitDesc;
        GPU_PROGRAM_DESC _pixelShaderBlitDesc;

        GPU_PROGRAM_DESC _vertexShaderSkyboxDesc;
        GPU_PROGRAM_DESC _pixelShaderSkyboxDesc;

        BLEND_STATE_DESC _blendOpaqueStateDesc;
        BLEND_STATE_DESC _blendTransparentStateDesc;

        RASTERIZER_STATE_DESC _rasterizerStateDesc;
        DEPTH_STENCIL_STATE_DESC _depthStencilStateDesc;

        SAMPLER_STATE_DESC _anisotropicSamplerStateDesc;
        SAMPLER_STATE_DESC _bilinearSamplerStateDesc;
    };

    /**	Provides easy access to BuiltinResources. */
    TE_CORE_EXPORT BuiltinResources& gBuiltinResources();
}

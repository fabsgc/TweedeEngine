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
        Transparent
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

        /** Returns anisotropic sampler state */
        SPtr<SamplerState> GetAnisotropicSamplerState() { return _anisotropicSamplerState; }

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

        void InitAnisotropicSampler();

        void InitShaderOpaque();
        void InitShaderTransparent();

    private:
        HShader _shaderOpaque;
        HShader _shaderTransparent;

        SPtr<SamplerState> _anisotropicSamplerState;

        SHADER_DESC _shaderDesc;
        GPU_PROGRAM_DESC _vertexShaderProgramDesc;
        GPU_PROGRAM_DESC _pixelShaderProgramDesc;

        BLEND_STATE_DESC _blendOpaqueStateDesc;
        BLEND_STATE_DESC _blendTransparentStateDesc;
        RASTERIZER_STATE_DESC _rasterizerStateDesc;
        DEPTH_STENCIL_STATE_DESC _depthStencilStateDesc;
        SAMPLER_STATE_DESC _samplerStateDesc;
    };

    /**	Provides easy access to BuiltinResources. */
    TE_CORE_EXPORT BuiltinResources& gBuiltinResources();
}

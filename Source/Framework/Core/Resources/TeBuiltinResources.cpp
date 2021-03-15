#include "Resources/TeBuiltinResources.h"
#include "Material/TePass.h"
#include "Material/TeMaterial.h"
#include "Material/TeTechnique.h"
#include "Utility/TeDataStream.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(BuiltinResources)

    BuiltinResources::BuiltinResources()
    { }

    BuiltinResources::~BuiltinResources()
    { 
        _anisotropicSamplerState = nullptr;
        _bilinearSamplerState = nullptr;
    }

    void BuiltinResources::OnStartUp()
    {
        InitGpuPrograms();
        InitStates();
        InitShaderDesc();
#if TE_PLATFORM == TE_PLATFORM_WIN32 //TODO to remove when OpenGL will be done
        InitSamplers();
        /*InitShaderOpaque();
        InitShaderTransparent();
        InitShaderBlit();
        InitShaderSkybox();
        InitShaderFXAA();
        InitShaderToneMapping();
        InitShaderBloom();
        InitShaderMotionBlur();
        InitShaderGaussianBlur();
        InitShaderPicking();
        InitShaderSelection();
        InitShaderHudPicking();
        InitShaderHudSelection();*/
        InitDefaultMaterial();
#endif
    }

    void BuiltinResources::OnShutDown()
    { }

    HShader BuiltinResources::GetBuiltinShader(BuiltinShader type)
    {
        HShader shader;

        switch(type)
        {
        case BuiltinShader::Opaque:
            if(!_shaderOpaque.IsLoaded())
                InitShaderOpaque();
            shader = _shaderOpaque;
            break;
        case BuiltinShader::Transparent:
            if(!_shaderTransparent.IsLoaded())
                InitShaderTransparent();
            shader = _shaderTransparent;
            break;
        case BuiltinShader::Blit:
            if(!_shaderBlit.IsLoaded())
                InitShaderBlit();
            shader = _shaderBlit;
            break;
        case BuiltinShader::Skybox:
            if(!_shaderSkybox.IsLoaded())
                InitShaderSkybox();
            shader = _shaderSkybox;
            break;
        case BuiltinShader::FXAA:
            if(!_shaderFXAA.IsLoaded())
                InitShaderFXAA();
            shader = _shaderFXAA;
            break;
        case BuiltinShader::ToneMapping:
            if(!_shaderToneMapping.IsLoaded())
                InitShaderToneMapping();
            shader = _shaderToneMapping;
            break;
        case BuiltinShader::Bloom:
            if(!_shaderBloom.IsLoaded())
                InitShaderBloom();
            shader = _shaderBloom;
            break;
        case BuiltinShader::MotionBlur:
            if(!_shaderMotionBlur.IsLoaded())
                InitShaderMotionBlur();
            shader = _shaderMotionBlur;
            break;
        case BuiltinShader::GaussianBlur:
            if(!_shaderGaussianBlur.IsLoaded())
                InitShaderGaussianBlur();
            shader = _shaderGaussianBlur;
            break;
        case BuiltinShader::Picking:
            if(!_shaderPicking.IsLoaded())
                InitShaderPicking();
            shader = _shaderPicking;
            break;
        case BuiltinShader::HudPicking:
            if(!_shaderHudPicking.IsLoaded())
                InitShaderHudPicking();
            shader = _shaderHudPicking;
            break;
        case BuiltinShader::Selection:
            if(!_shaderSelection.IsLoaded())
                InitShaderSelection();
            shader = _shaderSelection;
            break;
        case BuiltinShader::HudSelection:
            if(!_shaderHudSelection.IsLoaded())
                InitShaderHudSelection();
            shader = _shaderHudSelection;
            break;
        default:
            break;
        }

        return shader.GetNewHandleFromExisting();
    }

    SPtr<SamplerState> BuiltinResources::GetBuiltinSampler(BuiltinSampler type)
    {
        switch(type)
        {
        case BuiltinSampler::Anisotropic:
            return _anisotropicSamplerState;
        case BuiltinSampler::Bilinear:
            return _bilinearSamplerState; 
        case BuiltinSampler::Trilinear:
            return _trilinearSamplerState;
        default:
            break;
        }

        return nullptr;
    }

    void BuiltinResources::InitGpuPrograms()
    {
        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Forward_VS.hlsl"));
            _vertexShaderForwardDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderForwardDesc.FileName = "Forward_VS";
            _vertexShaderForwardDesc.EntryPoint = "main";
            _vertexShaderForwardDesc.Language = "hlsl";
            _vertexShaderForwardDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderForwardDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Forward_PS.hlsl"));
            _pixelShaderForwardDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderForwardDesc.FileName = "Forward_PS";
            _pixelShaderForwardDesc.EntryPoint = "main";
            _pixelShaderForwardDesc.Language = "hlsl";
            _pixelShaderForwardDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderForwardDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Blit_VS.hlsl"));
            _vertexShaderBlitDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderBlitDesc.FileName = "Blit_VS";
            _vertexShaderBlitDesc.EntryPoint = "main";
            _vertexShaderBlitDesc.Language = "hlsl";
            _vertexShaderBlitDesc.IncludePath = "";
            _vertexShaderBlitDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Blit_PS.hlsl"));
            _pixelShaderBlitDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderBlitDesc.FileName = "Blit_PS";
            _pixelShaderBlitDesc.EntryPoint = "main";
            _pixelShaderBlitDesc.Language = "hlsl";
            _pixelShaderBlitDesc.IncludePath = "";
            _pixelShaderBlitDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Skybox_VS.hlsl"));
            _vertexShaderSkyboxDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderSkyboxDesc.FileName = "Skybox_VS";
            _vertexShaderSkyboxDesc.EntryPoint = "main";
            _vertexShaderSkyboxDesc.Language = "hlsl";
            _vertexShaderSkyboxDesc.IncludePath = "";
            _vertexShaderSkyboxDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Skybox_PS.hlsl"));
            _pixelShaderSkyboxDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderSkyboxDesc.FileName = "Skybox_PS";
            _pixelShaderSkyboxDesc.EntryPoint = "main";
            _pixelShaderSkyboxDesc.Language = "hlsl";
            _pixelShaderSkyboxDesc.IncludePath = "";
            _pixelShaderSkyboxDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/FXAA_VS.hlsl"));
            _vertexShaderFXAADesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderFXAADesc.FileName = "FXAA_VS";
            _vertexShaderFXAADesc.EntryPoint = "main";
            _vertexShaderFXAADesc.Language = "hlsl";
            _vertexShaderFXAADesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderFXAADesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/FXAA_PS.hlsl"));
            _pixelShaderFXAADesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderFXAADesc.FileName = "FXAA_PS";
            _pixelShaderFXAADesc.EntryPoint = "main";
            _pixelShaderFXAADesc.Language = "hlsl";
            _pixelShaderFXAADesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderFXAADesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ToneMapping_VS.hlsl"));
            _vertexShaderToneMappingDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderToneMappingDesc.FileName = "ToneMapping_VS";
            _vertexShaderToneMappingDesc.EntryPoint = "main";
            _vertexShaderToneMappingDesc.Language = "hlsl";
            _vertexShaderToneMappingDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderToneMappingDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ToneMapping_PS.hlsl"));
            _pixelShaderToneMappingDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderToneMappingDesc.FileName = "ToneMapping_PS";
            _pixelShaderToneMappingDesc.EntryPoint = "main";
            _pixelShaderToneMappingDesc.Language = "hlsl";
            _pixelShaderToneMappingDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderToneMappingDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Bloom_VS.hlsl"));
            _vertexShaderBloomDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderBloomDesc.FileName = "Bloom_VS";
            _vertexShaderBloomDesc.EntryPoint = "main";
            _vertexShaderBloomDesc.Language = "hlsl";
            _vertexShaderBloomDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderBloomDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Bloom_PS.hlsl"));
            _pixelShaderBloomDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderBloomDesc.FileName = "Bloom_PS";
            _pixelShaderBloomDesc.EntryPoint = "main";
            _pixelShaderBloomDesc.Language = "hlsl";
            _pixelShaderBloomDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderBloomDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/MotionBlur_VS.hlsl"));
            _vertexShaderMotionBlurDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderMotionBlurDesc.FileName = "MotionBlur_VS";
            _vertexShaderMotionBlurDesc.EntryPoint = "main";
            _vertexShaderMotionBlurDesc.Language = "hlsl";
            _vertexShaderMotionBlurDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderMotionBlurDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/MotionBlur_PS.hlsl"));
            _pixelShaderMotionBlurDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderMotionBlurDesc.FileName = "MotionBlur_PS";
            _pixelShaderMotionBlurDesc.EntryPoint = "main";
            _pixelShaderMotionBlurDesc.Language = "hlsl";
            _pixelShaderMotionBlurDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderMotionBlurDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/GaussianBlur_VS.hlsl"));
            _vertexShaderGaussianBlurDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderGaussianBlurDesc.FileName = "GaussianBlur_VS";
            _vertexShaderGaussianBlurDesc.EntryPoint = "main";
            _vertexShaderGaussianBlurDesc.Language = "hlsl";
            _vertexShaderGaussianBlurDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderGaussianBlurDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/GaussianBlur_PS.hlsl"));
            _pixelShaderGaussianBlurDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderGaussianBlurDesc.FileName = "GaussianBlur_PS";
            _pixelShaderGaussianBlurDesc.EntryPoint = "main";
            _pixelShaderGaussianBlurDesc.Language = "hlsl";
            _pixelShaderGaussianBlurDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderGaussianBlurDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/PickSelect_VS.hlsl"));
            _vertexShaderPickSelectDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderPickSelectDesc.FileName = "PickSelect_VS";
            _vertexShaderPickSelectDesc.EntryPoint = "main";
            _vertexShaderPickSelectDesc.Language = "hlsl";
            _vertexShaderPickSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderPickSelectDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/PickSelect_PS.hlsl"));
            _pixelShaderPickSelectDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderPickSelectDesc.FileName = "PickSelect_PS";
            _pixelShaderPickSelectDesc.EntryPoint = "main";
            _pixelShaderPickSelectDesc.Language = "hlsl";
            _pixelShaderPickSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderPickSelectDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/HudPickSelect_VS.hlsl"));
            _vertexShaderHudPickSelectDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderHudPickSelectDesc.FileName = "HudPickSelect_VS";
            _vertexShaderHudPickSelectDesc.EntryPoint = "main";
            _vertexShaderHudPickSelectDesc.Language = "hlsl";
            _vertexShaderHudPickSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderHudPickSelectDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/HudPickSelect_GS.hlsl"));
            _geometryShaderHudPickSelectDesc.Type = GPT_GEOMETRY_PROGRAM;
            _geometryShaderHudPickSelectDesc.FileName = "HudPickSelect_GS";
            _geometryShaderHudPickSelectDesc.EntryPoint = "main";
            _geometryShaderHudPickSelectDesc.Language = "hlsl";
            _geometryShaderHudPickSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _geometryShaderHudPickSelectDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/HudPickSelect_PS.hlsl"));
            _pixelShaderHudPickSelectDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderHudPickSelectDesc.FileName = "HudPickSelect_PS";
            _pixelShaderHudPickSelectDesc.EntryPoint = "main";
            _pixelShaderHudPickSelectDesc.Language = "hlsl";
            _pixelShaderHudPickSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderHudPickSelectDesc.Source = shaderFile.GetAsString();
        }
    }
    void BuiltinResources::InitStates()
    {
        _blendTransparentStateDesc.AlphaToCoverageEnable = false;
        _blendTransparentStateDesc.IndependantBlendEnable = true;

        _rasterizerStateDesc.polygonMode = PM_SOLID;
        _rasterizerStateDesc.cullMode = CULL_CLOCKWISE;
        _rasterizerStateDesc.multisampleEnable = true;
        _rasterizerStateDesc.depthClipEnable = true;

        _depthStencilStateDesc.DepthReadEnable = true;
        _depthStencilStateDesc.DepthWriteEnable = true;
        _depthStencilStateDesc.StencilEnable = true;

        _depthStencilStateDesc.FrontStencilFailOp = SOP_KEEP;
        _depthStencilStateDesc.FrontStencilZFailOp = SOP_INCREMENT_WRAP;
        _depthStencilStateDesc.FrontStencilPassOp = SOP_KEEP;
        _depthStencilStateDesc.FrontStencilComparisonFunc = CMPF_ALWAYS_PASS;

        _depthStencilStateDesc.BackStencilFailOp = SOP_KEEP;
        _depthStencilStateDesc.BackStencilZFailOp = SOP_DECREMENT_WRAP;
        _depthStencilStateDesc.BackStencilPassOp = SOP_KEEP;
        _depthStencilStateDesc.BackStencilComparisonFunc = CMPF_ALWAYS_PASS;

        _anisotropicSamplerStateDesc.AddressMode = UVWAddressingMode();
        _anisotropicSamplerStateDesc.MinFilter = FO_ANISOTROPIC;
        _anisotropicSamplerStateDesc.MagFilter = FO_ANISOTROPIC;
        _anisotropicSamplerStateDesc.MipFilter = FO_ANISOTROPIC;
        _anisotropicSamplerStateDesc.MaxAnisotropy = 8;

        _bilinearSamplerStateDesc.AddressMode = UVWAddressingMode();
        _bilinearSamplerStateDesc.MinFilter = FO_LINEAR;
        _bilinearSamplerStateDesc.MagFilter = FO_LINEAR;
        _bilinearSamplerStateDesc.MipFilter = FO_POINT;

        _trilinearSamplerStateDesc.AddressMode = UVWAddressingMode();
        _trilinearSamplerStateDesc.MinFilter = FO_LINEAR;
        _trilinearSamplerStateDesc.MagFilter = FO_LINEAR;
        _trilinearSamplerStateDesc.MipFilter = FO_LINEAR;
    }

    void BuiltinResources::InitShaderDesc()
    {
        {
            SHADER_DATA_PARAM_DESC gViewDirDesc("gViewDir", "gViewDir", GPDT_FLOAT3);
            SHADER_DATA_PARAM_DESC gViewOriginDesc("gViewOrigin", "gViewOrigin", GPDT_FLOAT3);
            SHADER_DATA_PARAM_DESC gMatViewProjDesc("gMatViewProj", "gMatViewProj", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatViewDesc("gMatView", "gMatView", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatProjDesc("gMatProj", "gMatProj", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatPrevViewProjDesc("gMatPrevViewProj", "gMatPrevViewProj", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gNDCToPrevNDCDesc("gNDCToPrevNDC", "gNDCToPrevNDC", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gClipToUVScaleOffsetDesc("gClipToUVScaleOffset", "gClipToUVScaleOffset", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gUVToClipScaleOffsetDesc("gUVToClipScaleOffset", "gUVToClipScaleOffset", GPDT_FLOAT4);

            SHADER_DATA_PARAM_DESC gMatWorldDesc("gMatWorld", "gMatWorld", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatInvWorldDesc("gMatInvWorld", "gMatInvWorld", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatWorldNoScaleDesc("gMatWorldNoScale", "gMatWorldNoScale", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatInvWorldNoScaleDesc("gMatInvWorldNoScale", "gMatInvWorldNoScale", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatPrevWorldDesc("gMatPrevWorld", "gMatPrevWorld", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gLayerDesc("gLayer", "gLayer", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gHasAnimationDesc("gHasAnimation", "gHasAnimation", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gWriteVelocityDesc("gWriteVelocity", "gWriteVelocity", GPDT_INT1);

            SHADER_DATA_PARAM_DESC gTime("gTime", "gTime", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gFrameDeltaDesc("gFrameDelta", "gFrameDelta", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gSceneLightColorDesc("gSceneLightColor", "gSceneLightColor", GPDT_FLOAT4); 

            SHADER_DATA_PARAM_DESC gMatWorldViewProj("gMatWorldViewProj", "gMatWorldViewProj", GPDT_MATRIX_4X4);

            SHADER_DATA_PARAM_DESC gInstanceData("gInstanceData", "gInstanceData", GPDT_STRUCT);
            gInstanceData.ElementSize = sizeof(PerInstanceData);

            SHADER_DATA_PARAM_DESC gAmbient("gAmbient", "gAmbient", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gDiffuse("gDiffuse", "gDiffuse", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gEmissive("gEmissive", "gEmissive", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gSpecular("gSpecular", "gSpecular", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gTextureRepeat("gTextureRepeat", "gTextureRepeat", GPDT_FLOAT2);
            SHADER_DATA_PARAM_DESC gTextureOffset("gTextureOffset", "gTextureOffset", GPDT_FLOAT2);
            SHADER_DATA_PARAM_DESC gUseDiffuseMap("gUseDiffuseMap", "gUseDiffuseMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseEmissiveMap("gUseEmissiveMap", "gUseEmissiveMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseNormalMap("gUseNormalMap", "gUseNormalMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseSpecularMap("gUseSpecularMap", "gUseSpecularMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseBumpMap("gUseBumpMap", "gUseBumpMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseParallaxMap("gUseParallaxMap", "gUseParallaxMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseTransparencyMap("gUseTransparencyMap", "gUseTransparencyMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseReflectionMap("gUseReflectionMap", "gUseReflectionMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseOcclusionMap("gUseOcclusionMap", "gUseOcclusionMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseEnvironmentMap("gUseEnvironmentMap", "gUseEnvironmentMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gSpecularPower("gSpecularPower", "gSpecularPower", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gSpecularStrength("gSpecularStrength", "gSpecularStrength", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gTransparency("gTransparency", "gTransparency", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gIndexOfRefraction("gIndexOfRefraction", "gIndexOfRefraction", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gRefraction("gRefraction", "gRefraction", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gReflection("gReflection", "gReflection", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gBumpScale("gBumScale", "gBumScale", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gParallaxScale("gParallaxScale", "gParallaxScale", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gAlphaThreshold("gAlphaThreshold", "gAlphaThreshold", GPDT_FLOAT1);

            SHADER_OBJECT_PARAM_DESC anisotropicSamplerDesc("AnisotropicSampler", "AnisotropicSampler", GPOT_SAMPLER2D);

            SHADER_OBJECT_PARAM_DESC diffuseMapDesc("DiffuseMap", "DiffuseMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC emissiveMapDesc("EmissiveMap", "EmissiveMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC normalMapDesc("NormalMap", "NormalMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC specularMapDesc("SpecularMap", "SpecularMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC bumpMapDesc("BumpMap", "BumpMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC parallaxMapDesc("ParallaxMap", "ParallaxMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC transparencyMapDesc("TransparencyMap", "TransparencyMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC reflectionMapDesc("ReflectionMap", "ReflectionMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC occlusionMapDesc("OcclusionMap", "OcclusionMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC environmentMapDesc("EnvironmentMap", "EnvironmentMap", GPOT_TEXTURE2D);

            SHADER_DATA_PARAM_DESC gLightsDesc("gLights", "gLights", GPDT_STRUCT);
            gLightsDesc.ElementSize = sizeof(LightData);
            SHADER_DATA_PARAM_DESC gLightsNumberDesc("gLightsNumber", "gLightsNumber", GPDT_INT1);

            _forwardShaderDesc.AddParameter(gViewDirDesc);
            _forwardShaderDesc.AddParameter(gViewOriginDesc);
            _forwardShaderDesc.AddParameter(gMatViewProjDesc);
            _forwardShaderDesc.AddParameter(gMatViewDesc);
            _forwardShaderDesc.AddParameter(gMatProjDesc);
            _forwardShaderDesc.AddParameter(gMatPrevViewProjDesc);
            _forwardShaderDesc.AddParameter(gNDCToPrevNDCDesc);
            _forwardShaderDesc.AddParameter(gClipToUVScaleOffsetDesc);
            _forwardShaderDesc.AddParameter(gUVToClipScaleOffsetDesc);

            _forwardShaderDesc.AddParameter(gInstanceData);

            _forwardShaderDesc.AddParameter(gMatWorldDesc);
            _forwardShaderDesc.AddParameter(gMatInvWorldDesc);
            _forwardShaderDesc.AddParameter(gMatWorldNoScaleDesc);
            _forwardShaderDesc.AddParameter(gMatInvWorldNoScaleDesc);
            _forwardShaderDesc.AddParameter(gMatPrevWorldDesc);
            _forwardShaderDesc.AddParameter(gLayerDesc);
            _forwardShaderDesc.AddParameter(gHasAnimationDesc);
            _forwardShaderDesc.AddParameter(gWriteVelocityDesc);
            
            _forwardShaderDesc.AddParameter(gAmbient);
            _forwardShaderDesc.AddParameter(gDiffuse);
            _forwardShaderDesc.AddParameter(gEmissive);
            _forwardShaderDesc.AddParameter(gSpecular);
            _forwardShaderDesc.AddParameter(gTextureRepeat);
            _forwardShaderDesc.AddParameter(gTextureOffset);
            _forwardShaderDesc.AddParameter(gUseDiffuseMap);
            _forwardShaderDesc.AddParameter(gUseEmissiveMap);
            _forwardShaderDesc.AddParameter(gUseNormalMap);
            _forwardShaderDesc.AddParameter(gUseSpecularMap);
            _forwardShaderDesc.AddParameter(gUseBumpMap);
            _forwardShaderDesc.AddParameter(gUseParallaxMap);
            _forwardShaderDesc.AddParameter(gUseTransparencyMap);
            _forwardShaderDesc.AddParameter(gUseReflectionMap);
            _forwardShaderDesc.AddParameter(gUseOcclusionMap);
            _forwardShaderDesc.AddParameter(gUseEnvironmentMap);
            _forwardShaderDesc.AddParameter(gSpecularPower);
            _forwardShaderDesc.AddParameter(gSpecularStrength);
            _forwardShaderDesc.AddParameter(gTransparency);
            _forwardShaderDesc.AddParameter(gIndexOfRefraction);
            _forwardShaderDesc.AddParameter(gRefraction);
            _forwardShaderDesc.AddParameter(gReflection);
            _forwardShaderDesc.AddParameter(gBumpScale);
            _forwardShaderDesc.AddParameter(gParallaxScale);
            _forwardShaderDesc.AddParameter(gAlphaThreshold);

            _forwardShaderDesc.AddParameter(gTime);
            _forwardShaderDesc.AddParameter(gFrameDeltaDesc);
            _forwardShaderDesc.AddParameter(gSceneLightColorDesc);

            _forwardShaderDesc.AddParameter(gMatWorldViewProj);

            _forwardShaderDesc.AddParameter(anisotropicSamplerDesc);

            _forwardShaderDesc.AddParameter(diffuseMapDesc);
            _forwardShaderDesc.AddParameter(emissiveMapDesc);
            _forwardShaderDesc.AddParameter(normalMapDesc);
            _forwardShaderDesc.AddParameter(specularMapDesc);
            _forwardShaderDesc.AddParameter(bumpMapDesc);
            _forwardShaderDesc.AddParameter(parallaxMapDesc);
            _forwardShaderDesc.AddParameter(transparencyMapDesc);
            _forwardShaderDesc.AddParameter(reflectionMapDesc);
            _forwardShaderDesc.AddParameter(occlusionMapDesc);
            _forwardShaderDesc.AddParameter(environmentMapDesc);

            _forwardShaderDesc.AddParameter(gLightsDesc);
            _forwardShaderDesc.AddParameter(gLightsNumberDesc);
        }

        {
            SHADER_DATA_PARAM_DESC gMSAACountDesc("gMSAACount", "gMSAACount", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gIsDepthDesc("gIsDepth", "gIsDepth", GPDT_INT1);

            SHADER_OBJECT_PARAM_DESC bilinearSamplerDesc("BilinearSampler", "BilinearSampler", GPOT_SAMPLER2D);
            SHADER_OBJECT_PARAM_DESC sourceMapDesc("SourceMap", "SourceMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC SourceMapMSDesc("SourceMapMS", "SourceMapMS", GPOT_RWTEXTURE2DMS);
            SHADER_OBJECT_PARAM_DESC SourceMapMSDepthDesc("SourceMapMSDepth", "SourceMapMSDepth", GPOT_RWTEXTURE2DMS);

            _blitShaderDesc.AddParameter(gMSAACountDesc);
            _blitShaderDesc.AddParameter(gIsDepthDesc);

            _blitShaderDesc.AddParameter(bilinearSamplerDesc);

            _blitShaderDesc.AddParameter(sourceMapDesc);
            _blitShaderDesc.AddParameter(SourceMapMSDesc);
            _blitShaderDesc.AddParameter(SourceMapMSDepthDesc);
        }

        {
            SHADER_DATA_PARAM_DESC gClearColor("gClearColor", "gClearColor", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gUseTexture("gUseTexture", "gUseTexture", GPDT_INT1);

            SHADER_OBJECT_PARAM_DESC bilinearSamplerDesc("BilinearSampler", "BilinearSampler", GPOT_SAMPLER2D);

            SHADER_OBJECT_PARAM_DESC textureMapDesc("TextureMap", "TextureMap", GPOT_TEXTURE2D);

            _skyboxShaderDesc.AddParameter(gClearColor);
            _skyboxShaderDesc.AddParameter(gUseTexture);
            
            _skyboxShaderDesc.AddParameter(bilinearSamplerDesc);

            _skyboxShaderDesc.AddParameter(textureMapDesc);
        }

        {
            SHADER_DATA_PARAM_DESC gInvTexSizeDesc("gInvTexSize", "gInvTexSize", GPDT_FLOAT2);
            SHADER_OBJECT_PARAM_DESC bilinearSamplerDesc("BilinearSampler", "BilinearSampler", GPOT_SAMPLER2D);
            SHADER_OBJECT_PARAM_DESC sourceMapDesc("SourceMap", "SourceMap", GPOT_TEXTURE2D);

            _FXAAShaderDesc.AddParameter(gInvTexSizeDesc);
            _FXAAShaderDesc.AddParameter(bilinearSamplerDesc);
            _FXAAShaderDesc.AddParameter(sourceMapDesc);
        }

        {
            SHADER_DATA_PARAM_DESC gGammaDesc("gGamma", "gGamma", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gExposureDesc("gExposure", "gExposure", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gContrastDesc("gContrast", "gContrast", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gBrightnessDesc("gBrightness", "gBrightness", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gMSAACountDesc("gMSAACount", "gMSAACount", GPDT_INT1);

            SHADER_OBJECT_PARAM_DESC bilinearSamplerDesc("BilinearSampler", "BilinearSampler", GPOT_SAMPLER2D);

            SHADER_OBJECT_PARAM_DESC sourceMapDesc("SourceMap", "SourceMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC SourceMapMSDesc("SourceMapMS", "SourceMapMS", GPOT_RWTEXTURE2DMS);

            _toneMappingShaderDesc.AddParameter(gGammaDesc);
            _toneMappingShaderDesc.AddParameter(gExposureDesc);
            _toneMappingShaderDesc.AddParameter(gContrastDesc);
            _toneMappingShaderDesc.AddParameter(gBrightnessDesc);
            _toneMappingShaderDesc.AddParameter(gMSAACountDesc);

            _toneMappingShaderDesc.AddParameter(bilinearSamplerDesc);

            _toneMappingShaderDesc.AddParameter(sourceMapDesc);
            _toneMappingShaderDesc.AddParameter(SourceMapMSDesc);
        }

        {
            SHADER_DATA_PARAM_DESC gMSAACountDesc("gMSAACount", "gMSAACount", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gTintDesc("gTint", "gTint", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gIntensityDesc("gIntensity", "gIntensity", GPDT_FLOAT1);

            SHADER_OBJECT_PARAM_DESC bilinearSamplerDesc("BilinearSampler", "BilinearSampler", GPOT_SAMPLER2D);

            SHADER_OBJECT_PARAM_DESC sourceMapDesc("SourceMap", "SourceMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC sourceMapMSDesc("SourceMapMS", "SourceMapMS", GPOT_RWTEXTURE2DMS);

            SHADER_OBJECT_PARAM_DESC emissiveMapDesc("EmissiveMap", "EmissiveMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC emissiveMapMSDesc("EmissiveMapMS", "EmissiveMapMS", GPOT_RWTEXTURE2DMS);

            _bloomShaderDesc.AddParameter(gMSAACountDesc);
            _bloomShaderDesc.AddParameter(gTintDesc);
            _bloomShaderDesc.AddParameter(gIntensityDesc);

            _bloomShaderDesc.AddParameter(bilinearSamplerDesc);

            _bloomShaderDesc.AddParameter(sourceMapDesc);
            _bloomShaderDesc.AddParameter(sourceMapMSDesc);

            _bloomShaderDesc.AddParameter(emissiveMapDesc);
            _bloomShaderDesc.AddParameter(emissiveMapMSDesc);
        }

        {
            SHADER_DATA_PARAM_DESC gViewDirDesc("gViewDir", "gViewDir", GPDT_FLOAT3);
            SHADER_DATA_PARAM_DESC gViewOriginDesc("gViewOrigin", "gViewOrigin", GPDT_FLOAT3);
            SHADER_DATA_PARAM_DESC gMatViewProjDesc("gMatViewProj", "gMatViewProj", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatViewDesc("gMatView", "gMatView", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatProjDesc("gMatProj", "gMatProj", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatPrevViewProjDesc("gMatPrevViewProj", "gMatPrevViewProj", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gNDCToPrevNDCDesc("gNDCToPrevNDC", "gNDCToPrevNDC", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gClipToUVScaleOffsetDesc("gClipToUVScaleOffset", "gClipToUVScaleOffset", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gUVToClipScaleOffsetDesc("gUVToClipScaleOffset", "gUVToClipScaleOffset", GPDT_FLOAT4);

            SHADER_DATA_PARAM_DESC gFrameDeltaDesc("gFrameDelta", "gFrameDelta", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gHalfNumSamplesDesc("gHalfNumSamples", "gHalfNumSamples", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gMSAACountDesc("gMSAACount", "gMSAACount", GPDT_INT1);

            SHADER_OBJECT_PARAM_DESC bilinearSamplerDesc("BilinearSampler", "BilinearSampler", GPOT_SAMPLER2D);

            SHADER_OBJECT_PARAM_DESC sourceMapDesc("SourceMap", "SourceMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC SourceMapMSDesc("SourceMapMS", "SourceMapMS", GPOT_RWTEXTURE2DMS);

            SHADER_OBJECT_PARAM_DESC depthMapDesc("DepthMap", "DepthMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC depthMapMSDesc("DepthMapMS", "DepthMapMS", GPOT_RWTEXTURE2DMS);

            SHADER_OBJECT_PARAM_DESC velocityMapDesc("VelocityMap", "VelocityMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC velocityMapMSDesc("VelocityMapMS", "VelocityMapMS", GPOT_RWTEXTURE2DMS);

            _motionBlurShaderDesc.AddParameter(gViewDirDesc);
            _motionBlurShaderDesc.AddParameter(gViewOriginDesc);
            _motionBlurShaderDesc.AddParameter(gMatViewProjDesc);
            _motionBlurShaderDesc.AddParameter(gMatViewDesc);
            _motionBlurShaderDesc.AddParameter(gMatProjDesc);
            _motionBlurShaderDesc.AddParameter(gMatPrevViewProjDesc);
            _motionBlurShaderDesc.AddParameter(gNDCToPrevNDCDesc);
            _motionBlurShaderDesc.AddParameter(gClipToUVScaleOffsetDesc);
            _motionBlurShaderDesc.AddParameter(gUVToClipScaleOffsetDesc);

            _motionBlurShaderDesc.AddParameter(gFrameDeltaDesc);
            _motionBlurShaderDesc.AddParameter(gHalfNumSamplesDesc);
            _motionBlurShaderDesc.AddParameter(gMSAACountDesc);

            _motionBlurShaderDesc.AddParameter(bilinearSamplerDesc);

            _motionBlurShaderDesc.AddParameter(sourceMapDesc);
            _motionBlurShaderDesc.AddParameter(SourceMapMSDesc);

            _motionBlurShaderDesc.AddParameter(depthMapDesc);
            _motionBlurShaderDesc.AddParameter(depthMapMSDesc);

            _motionBlurShaderDesc.AddParameter(velocityMapDesc);
            _motionBlurShaderDesc.AddParameter(velocityMapMSDesc);
        }

        {
            SHADER_DATA_PARAM_DESC gSourceDimensionsDesc("gSourceDimensions", "gSourceDimensions", GPDT_FLOAT2);
            SHADER_DATA_PARAM_DESC gMSAACountDesc("gMSAACount", "gMSAACount", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gHorizontalDesc("gHorizontal", "gHorizontal", GPDT_INT1);

            SHADER_OBJECT_PARAM_DESC bilinearSamplerDesc("BilinearSampler", "BilinearSampler", GPOT_SAMPLER2D);

            SHADER_OBJECT_PARAM_DESC sourceMapDesc("SourceMap", "SourceMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC SourceMapMSDesc("SourceMapMS", "SourceMapMS", GPOT_RWTEXTURE2DMS);
            
            _gaussianBlurShaderDesc.AddParameter(bilinearSamplerDesc);

            _gaussianBlurShaderDesc.AddParameter(sourceMapDesc);
            _gaussianBlurShaderDesc.AddParameter(SourceMapMSDesc);

            _gaussianBlurShaderDesc.AddParameter(gSourceDimensionsDesc);
            _gaussianBlurShaderDesc.AddParameter(gMSAACountDesc);
            _gaussianBlurShaderDesc.AddParameter(gHorizontalDesc);
        }

        {
            SHADER_DATA_PARAM_DESC gMatViewProjDesc("gMatViewProj", "gMatViewProj", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatViewOriginDesc("gViewOrigin", "gViewOrigin", GPDT_FLOAT3);
            SHADER_DATA_PARAM_DESC gRenderTypeDesc("gRenderType", "gRenderType", GPDT_INT1);
            
            SHADER_DATA_PARAM_DESC gMatWorldDesc("gMatWorld", "gMatWorld", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gColorDesc("gColor", "gColor", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gHasAnimationDesc("gHasAnimation", "gHasAnimation", GPDT_INT1);

            _pickSelectShaderDesc.AddParameter(gMatViewProjDesc);
            _hudPickSelectShaderDesc.AddParameter(gMatViewOriginDesc);
            _pickSelectShaderDesc.AddParameter(gRenderTypeDesc);

            _pickSelectShaderDesc.AddParameter(gMatWorldDesc);
            _pickSelectShaderDesc.AddParameter(gColorDesc);
            _pickSelectShaderDesc.AddParameter(gHasAnimationDesc);
        }

        {
            SHADER_DATA_PARAM_DESC gMatViewProjDesc("gMatViewProj", "gMatViewProj", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatViewOriginDesc("gViewOrigin", "gViewOrigin", GPDT_FLOAT3);
            SHADER_DATA_PARAM_DESC gRenderTypeDesc("gRenderType", "gRenderType", GPDT_INT1);

            SHADER_DATA_PARAM_DESC gInstanceData("gInstanceData", "gInstanceData", GPDT_STRUCT);
            gInstanceData.ElementSize = sizeof(PerInstanceData);

            SHADER_OBJECT_PARAM_DESC anisotropicSamplerDesc("AnisotropicSampler", "AnisotropicSampler", GPOT_SAMPLER2D);
            SHADER_OBJECT_PARAM_DESC maskTextureDesc("MaskTexture", "MaskTexture", GPOT_TEXTURE2D);

            _hudPickSelectShaderDesc.AddParameter(gMatViewProjDesc);
            _hudPickSelectShaderDesc.AddParameter(gMatViewOriginDesc);
            _hudPickSelectShaderDesc.AddParameter(gRenderTypeDesc);

            _hudPickSelectShaderDesc.AddParameter(gInstanceData);

            _hudPickSelectShaderDesc.AddParameter(anisotropicSamplerDesc);
            _hudPickSelectShaderDesc.AddParameter(maskTextureDesc);
        }
    }

    void BuiltinResources::InitSamplers()
    {
        _anisotropicSamplerState = SamplerState::Create(_anisotropicSamplerStateDesc);
        _bilinearSamplerState = SamplerState::Create(_bilinearSamplerStateDesc);
        _trilinearSamplerState = SamplerState::Create(_trilinearSamplerStateDesc);
    }

    void BuiltinResources::InitShaderOpaque()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderForwardDesc;
        passDesc.PixelProgramDesc = _pixelShaderForwardDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _forwardShaderDesc;
        shaderDesc.QueueType = QueueSortType::FrontToBack;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderOpaque = Shader::Create("ForwardOpaque", shaderDesc);
    }

    void BuiltinResources::InitShaderTransparent()
    {
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendEnable = true;
        _blendTransparentStateDesc.RenderTargetDesc[0].SrcBlend = BlendFactor::BF_SOURCE_ALPHA;
        _blendTransparentStateDesc.RenderTargetDesc[0].DstBlend = BlendFactor::BF_INV_SOURCE_ALPHA;
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendOp = BlendOperation::BO_ADD;
        _blendTransparentStateDesc.RenderTargetDesc[0].SrcBlendAlpha = BlendFactor::BF_ZERO;
        _blendTransparentStateDesc.RenderTargetDesc[0].DstBlendAlpha = BlendFactor::BF_ONE;
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendOpAlpha = BlendOperation::BO_ADD;
        _blendTransparentStateDesc.RenderTargetDesc[0].RenderTargetWriteMask = 0x0f;

        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendTransparentStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderForwardDesc;
        passDesc.PixelProgramDesc = _pixelShaderForwardDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_CLOCKWISE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _forwardShaderDesc;
        shaderDesc.Flags = (UINT32)ShaderFlag::Transparent;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderTransparent = Shader::Create("ForwardTransparent", shaderDesc);
    }

    void BuiltinResources::InitShaderBlit()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderBlitDesc;
        passDesc.PixelProgramDesc = _pixelShaderBlitDesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CULL_NONE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _blitShaderDesc;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderBlit = Shader::Create("Blit", shaderDesc);
    }

    void BuiltinResources::InitShaderSkybox()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderSkyboxDesc;
        passDesc.PixelProgramDesc = _pixelShaderSkyboxDesc;

        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _skyboxShaderDesc;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderSkybox = Shader::Create("Skybox", shaderDesc);
    }

    void BuiltinResources::InitShaderFXAA()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderFXAADesc;
        passDesc.PixelProgramDesc = _pixelShaderFXAADesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CULL_NONE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _FXAAShaderDesc;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderFXAA = Shader::Create("FXAA", shaderDesc);
    }

    void BuiltinResources::InitShaderToneMapping()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderToneMappingDesc;
        passDesc.PixelProgramDesc = _pixelShaderToneMappingDesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CULL_NONE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _toneMappingShaderDesc;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderToneMapping = Shader::Create("ToneMapping", shaderDesc);
    }

    void BuiltinResources::InitShaderBloom()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderBloomDesc;
        passDesc.PixelProgramDesc = _pixelShaderBloomDesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CULL_NONE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _bloomShaderDesc;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderBloom = Shader::Create("Bloom", shaderDesc);
    }

    void BuiltinResources::InitShaderMotionBlur()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderMotionBlurDesc;
        passDesc.PixelProgramDesc = _pixelShaderMotionBlurDesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CULL_NONE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _motionBlurShaderDesc;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderMotionBlur = Shader::Create("MotionBlur", shaderDesc);
    }

    void BuiltinResources::InitShaderGaussianBlur()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderGaussianBlurDesc;
        passDesc.PixelProgramDesc = _pixelShaderGaussianBlurDesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CULL_NONE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _motionBlurShaderDesc;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderGaussianBlur = Shader::Create("GaussianBlur", shaderDesc);
    }

    void BuiltinResources::InitShaderPicking()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderPickSelectDesc;
        passDesc.PixelProgramDesc = _pixelShaderPickSelectDesc;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_CLOCKWISE;

        SHADER_DESC shaderDesc = _pickSelectShaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderPicking = Shader::Create("Picking", shaderDesc);
    }

    void BuiltinResources::InitShaderSelection()
    {
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendEnable = true;
        _blendTransparentStateDesc.RenderTargetDesc[0].SrcBlend = BlendFactor::BF_SOURCE_ALPHA;
        _blendTransparentStateDesc.RenderTargetDesc[0].DstBlend = BlendFactor::BF_INV_SOURCE_ALPHA;
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendOp = BlendOperation::BO_MAX;
        _blendTransparentStateDesc.RenderTargetDesc[0].SrcBlendAlpha = BlendFactor::BF_ZERO;
        _blendTransparentStateDesc.RenderTargetDesc[0].DstBlendAlpha = BlendFactor::BF_ONE;
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendOpAlpha = BlendOperation::BO_ADD;
        _blendTransparentStateDesc.RenderTargetDesc[0].RenderTargetWriteMask = 0x0f;

        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendTransparentStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderPickSelectDesc;
        passDesc.PixelProgramDesc = _pixelShaderPickSelectDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_CLOCKWISE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _pickSelectShaderDesc;
        shaderDesc.Flags = (UINT32)ShaderFlag::Transparent;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderSelection = Shader::Create("Picking", shaderDesc);
    }

    void BuiltinResources::InitShaderHudPicking()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderHudPickSelectDesc;
        passDesc.GeometryProgramDesc = _geometryShaderHudPickSelectDesc;
        passDesc.PixelProgramDesc = _pixelShaderHudPickSelectDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _hudPickSelectShaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderHudPicking = Shader::Create("HudPicking", shaderDesc);
    }

    void BuiltinResources::InitShaderHudSelection()
    {
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendEnable = true;
        _blendTransparentStateDesc.RenderTargetDesc[0].SrcBlend = BlendFactor::BF_SOURCE_ALPHA;
        _blendTransparentStateDesc.RenderTargetDesc[0].DstBlend = BlendFactor::BF_INV_SOURCE_ALPHA;
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendOp = BlendOperation::BO_ADD;
        _blendTransparentStateDesc.RenderTargetDesc[0].SrcBlendAlpha = BlendFactor::BF_ZERO;
        _blendTransparentStateDesc.RenderTargetDesc[0].DstBlendAlpha = BlendFactor::BF_ONE;
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendOpAlpha = BlendOperation::BO_ADD;
        _blendTransparentStateDesc.RenderTargetDesc[0].RenderTargetWriteMask = 0x0f;

        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendTransparentStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderHudPickSelectDesc;
        passDesc.GeometryProgramDesc = _geometryShaderHudPickSelectDesc;
        passDesc.PixelProgramDesc = _pixelShaderHudPickSelectDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _hudPickSelectShaderDesc;
        shaderDesc.Flags = (UINT32)ShaderFlag::Transparent;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderHudSelection = Shader::Create("HudSelection", shaderDesc);
    }

    void BuiltinResources::InitDefaultMaterial()
    {
        MaterialProperties properties;
        _defaultMaterial = Material::Create(GetBuiltinShader(BuiltinShader::Opaque));
        _defaultMaterial->SetProperties(properties);
    }

    BuiltinResources& gBuiltinResources()
    {
        return BuiltinResources::Instance();
    }
}

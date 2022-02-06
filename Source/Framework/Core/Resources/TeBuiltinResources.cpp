#include "Resources/TeBuiltinResources.h"
#include "Resources/TeResourceManager.h"
#include "Image/TeTexture.h"
#include "Material/TePass.h"
#include "Material/TeMaterial.h"
#include "Material/TeTechnique.h"
#include "Utility/TeDataStream.h"
#include "Importer/TeTextureImportOptions.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(BuiltinResources)

    BuiltinResources::BuiltinResources()
    { }

    BuiltinResources::~BuiltinResources()
    { 
        _anisotropicSamplerState = nullptr;
        _bilinearSamplerState = nullptr;
        _trilinearSamplerState = nullptr;
        _frameworkIcon = nullptr;
    }

    void BuiltinResources::OnStartUp()
    {
        InitGpuPrograms();
        InitStates();
        InitShaderDesc();
#if TE_PLATFORM == TE_PLATFORM_WIN32 // TODO to remove when OpenGL will be done
        InitSamplers();
        InitDefaultMaterial();
        InitFrameworkIcon();
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
        case BuiltinShader::TransparentCullNone:
            if(!_shaderTransparentCullNone.IsLoaded())
                InitShaderTransparent(false);
            shader = _shaderTransparentCullNone;
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
        case BuiltinShader::BulletDebug:
            if (!_shaderBulletDebug.IsLoaded())
                InitShaderBulletDebug();
            shader = _shaderBulletDebug;
            break;
        case BuiltinShader::PreviewOpaque:
            if (!_shaderOpaque.IsLoaded())
                InitShaderOpaque();
            shader = _shaderOpaque;
            break;
        case BuiltinShader::PreviewTransparent:
            if(!_shaderTransparent.IsLoaded())
                InitShaderTransparent();
            shader = _shaderTransparent;
            break;
        case BuiltinShader::SSAO:
            if (!_shaderSSAO.IsLoaded())
                InitShaderSSAO();
            shader = _shaderSSAO;
            break;
        case BuiltinShader::SSAOBlur:
            if (!_shaderSSAOBlur.IsLoaded())
                InitShaderSSAOBlur();
            shader = _shaderSSAOBlur;
            break;
        case BuiltinShader::SSAODownSample:
            if (!_shaderSSAODownSample.IsLoaded())
                InitShaderSSAODownSample();
            shader = _shaderSSAODownSample;
            break;
        case BuiltinShader::Decal:
            if (!_shaderDecal.IsLoaded())
                InitShaderDecal();
            shader = _shaderDecal;
            break;
        default:
            TE_ASSERT_ERROR(false, "Can't find \"" + ToString((UINT32)type) + "\" shader.")
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
            TE_ASSERT_ERROR(false, "Can't find \"" + ToString((UINT32)type) + "\" sampler.")
            break;
        }

        return nullptr;
    }

    const PixelData& BuiltinResources::GetFrameworkIcon()
    {
        return *_frameworkIcon.get();
    }

    void BuiltinResources::InitGpuPrograms()
    {
        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Forward_VS.hlsl"));
            _vertexShaderForwardDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderForwardDesc.FilePath = SHADERS_FOLDER + String("HLSL/Forward_VS.hlsl");
            _vertexShaderForwardDesc.EntryPoint = "main";
            _vertexShaderForwardDesc.Language = "hlsl";
            _vertexShaderForwardDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderForwardDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Forward_PS.hlsl"));
            _pixelShaderForwardDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderForwardDesc.FilePath = SHADERS_FOLDER + String("HLSL/Forward_PS.hlsl");
            _pixelShaderForwardDesc.EntryPoint = "main";
            _pixelShaderForwardDesc.Language = "hlsl";
            _pixelShaderForwardDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderForwardDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Blit_VS.hlsl"));
            _vertexShaderBlitDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderBlitDesc.FilePath = SHADERS_FOLDER + String("HLSL/Blit_VS.hlsl");
            _vertexShaderBlitDesc.EntryPoint = "main";
            _vertexShaderBlitDesc.Language = "hlsl";
            _vertexShaderBlitDesc.IncludePath = "";
            _vertexShaderBlitDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Blit_PS.hlsl"));
            _pixelShaderBlitDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderBlitDesc.FilePath = SHADERS_FOLDER + String("HLSL/Blit_PS.hlsl");
            _pixelShaderBlitDesc.EntryPoint = "main";
            _pixelShaderBlitDesc.Language = "hlsl";
            _pixelShaderBlitDesc.IncludePath = "";
            _pixelShaderBlitDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Skybox_VS.hlsl"));
            _vertexShaderSkyboxDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderSkyboxDesc.FilePath = SHADERS_FOLDER + String("HLSL/Skybox_VS.hlsl");
            _vertexShaderSkyboxDesc.EntryPoint = "main";
            _vertexShaderSkyboxDesc.Language = "hlsl";
            _vertexShaderSkyboxDesc.IncludePath = "";
            _vertexShaderSkyboxDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Skybox_PS.hlsl"));
            _pixelShaderSkyboxDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderSkyboxDesc.FilePath = SHADERS_FOLDER + String("HLSL/Skybox_PS.hlsl");
            _pixelShaderSkyboxDesc.EntryPoint = "main";
            _pixelShaderSkyboxDesc.Language = "hlsl";
            _pixelShaderSkyboxDesc.IncludePath = "";
            _pixelShaderSkyboxDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/FXAA_VS.hlsl"));
            _vertexShaderFXAADesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderFXAADesc.FilePath = SHADERS_FOLDER + String("HLSL/FXAA_VS.hlsl");
            _vertexShaderFXAADesc.EntryPoint = "main";
            _vertexShaderFXAADesc.Language = "hlsl";
            _vertexShaderFXAADesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderFXAADesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/FXAA_PS.hlsl"));
            _pixelShaderFXAADesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderFXAADesc.FilePath = SHADERS_FOLDER + String("HLSL/FXAA_PS.hlsl");
            _pixelShaderFXAADesc.EntryPoint = "main";
            _pixelShaderFXAADesc.Language = "hlsl";
            _pixelShaderFXAADesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderFXAADesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ToneMapping_VS.hlsl"));
            _vertexShaderToneMappingDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderToneMappingDesc.FilePath = SHADERS_FOLDER + String("HLSL/ToneMapping_VS.hlsl");
            _vertexShaderToneMappingDesc.EntryPoint = "main";
            _vertexShaderToneMappingDesc.Language = "hlsl";
            _vertexShaderToneMappingDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderToneMappingDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ToneMapping_PS.hlsl"));
            _pixelShaderToneMappingDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderToneMappingDesc.FilePath = SHADERS_FOLDER + String("HLSL/ToneMapping_PS.hlsl");
            _pixelShaderToneMappingDesc.EntryPoint = "main";
            _pixelShaderToneMappingDesc.Language = "hlsl";
            _pixelShaderToneMappingDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderToneMappingDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Bloom_VS.hlsl"));
            _vertexShaderBloomDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderBloomDesc.FilePath = SHADERS_FOLDER + String("HLSL/Bloom_VS.hlsl");
            _vertexShaderBloomDesc.EntryPoint = "main";
            _vertexShaderBloomDesc.Language = "hlsl";
            _vertexShaderBloomDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderBloomDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Bloom_PS.hlsl"));
            _pixelShaderBloomDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderBloomDesc.FilePath = SHADERS_FOLDER + String("HLSL/Bloom_PS.hlsl");
            _pixelShaderBloomDesc.EntryPoint = "main";
            _pixelShaderBloomDesc.Language = "hlsl";
            _pixelShaderBloomDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderBloomDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/MotionBlur_VS.hlsl"));
            _vertexShaderMotionBlurDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderMotionBlurDesc.FilePath = SHADERS_FOLDER + String("HLSL/MotionBlur_VS.hlsl");
            _vertexShaderMotionBlurDesc.EntryPoint = "main";
            _vertexShaderMotionBlurDesc.Language = "hlsl";
            _vertexShaderMotionBlurDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderMotionBlurDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/MotionBlur_PS.hlsl"));
            _pixelShaderMotionBlurDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderMotionBlurDesc.FilePath = SHADERS_FOLDER + String("HLSL/MotionBlur_PS.hlsl");
            _pixelShaderMotionBlurDesc.EntryPoint = "main";
            _pixelShaderMotionBlurDesc.Language = "hlsl";
            _pixelShaderMotionBlurDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderMotionBlurDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/GaussianBlur_VS.hlsl"));
            _vertexShaderGaussianBlurDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderGaussianBlurDesc.FilePath = SHADERS_FOLDER + String("HLSL/GaussianBlur_VS.hlsl");
            _vertexShaderGaussianBlurDesc.EntryPoint = "main";
            _vertexShaderGaussianBlurDesc.Language = "hlsl";
            _vertexShaderGaussianBlurDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderGaussianBlurDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/GaussianBlur_PS.hlsl"));
            _pixelShaderGaussianBlurDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderGaussianBlurDesc.FilePath = SHADERS_FOLDER + String("HLSL/GaussianBlur_PS.hlsl");
            _pixelShaderGaussianBlurDesc.EntryPoint = "main";
            _pixelShaderGaussianBlurDesc.Language = "hlsl";
            _pixelShaderGaussianBlurDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderGaussianBlurDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/PickSelect_VS.hlsl"));
            _vertexShaderPickSelectDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderPickSelectDesc.FilePath = SHADERS_FOLDER + String("HLSL/PickSelect_VS.hlsl");
            _vertexShaderPickSelectDesc.EntryPoint = "main";
            _vertexShaderPickSelectDesc.Language = "hlsl";
            _vertexShaderPickSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderPickSelectDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/PickSelect_PS.hlsl"));
            _pixelShaderPickSelectDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderPickSelectDesc.FilePath = SHADERS_FOLDER + String("HLSL/PickSelect_PS.hlsl");
            _pixelShaderPickSelectDesc.EntryPoint = "main";
            _pixelShaderPickSelectDesc.Language = "hlsl";
            _pixelShaderPickSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderPickSelectDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/HudPickSelect_VS.hlsl"));
            _vertexShaderHudPickSelectDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderHudPickSelectDesc.FilePath = SHADERS_FOLDER + String("HLSL/HudPickSelect_VS.hlsl");
            _vertexShaderHudPickSelectDesc.EntryPoint = "main";
            _vertexShaderHudPickSelectDesc.Language = "hlsl";
            _vertexShaderHudPickSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderHudPickSelectDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/HudPickSelect_GS.hlsl"));
            _geometryShaderHudPickSelectDesc.Type = GPT_GEOMETRY_PROGRAM;
            _geometryShaderHudPickSelectDesc.FilePath = SHADERS_FOLDER + String("HLSL/HudPickSelect_GS.hlsl");
            _geometryShaderHudPickSelectDesc.EntryPoint = "main";
            _geometryShaderHudPickSelectDesc.Language = "hlsl";
            _geometryShaderHudPickSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _geometryShaderHudPickSelectDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/HudPickSelect_PS.hlsl"));
            _pixelShaderHudPickSelectDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderHudPickSelectDesc.FilePath = SHADERS_FOLDER + String("HLSL/HudPickSelect_PS.hlsl");
            _pixelShaderHudPickSelectDesc.EntryPoint = "main";
            _pixelShaderHudPickSelectDesc.Language = "hlsl";
            _pixelShaderHudPickSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderHudPickSelectDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/BulletDebug_VS.hlsl"));
            _vertexShaderBulletDebugDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderBulletDebugDesc.FilePath = SHADERS_FOLDER + String("HLSL/BulletDebug_VS.hlsl");
            _vertexShaderBulletDebugDesc.EntryPoint = "main";
            _vertexShaderBulletDebugDesc.Language = "hlsl";
            _vertexShaderBulletDebugDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderBulletDebugDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/BulletDebug_GS.hlsl"));
            _geometryShaderBulletDebugDesc.Type = GPT_GEOMETRY_PROGRAM;
            _geometryShaderBulletDebugDesc.FilePath = SHADERS_FOLDER + String("HLSL/BulletDebug_GS.hlsl");
            _geometryShaderBulletDebugDesc.EntryPoint = "main";
            _geometryShaderBulletDebugDesc.Language = "hlsl";
            _geometryShaderBulletDebugDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _geometryShaderBulletDebugDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/BulletDebug_PS.hlsl"));
            _pixelShaderBulletDebugDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderBulletDebugDesc.FilePath = SHADERS_FOLDER + String("HLSL/BulletDebug_PS.hlsl");
            _pixelShaderBulletDebugDesc.EntryPoint = "main";
            _pixelShaderBulletDebugDesc.Language = "hlsl";
            _pixelShaderBulletDebugDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderBulletDebugDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/SSAO_VS.hlsl"));
            _vertexShaderSSAODesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderSSAODesc.FilePath = SHADERS_FOLDER + String("HLSL/SSAO_VS.hlsl");
            _vertexShaderSSAODesc.EntryPoint = "main";
            _vertexShaderSSAODesc.Language = "hlsl";
            _vertexShaderSSAODesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderSSAODesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/SSAO_PS.hlsl"));
            _pixelShaderSSAODesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderSSAODesc.FilePath = SHADERS_FOLDER + String("HLSL/SSAO_PS.hlsl");
            _pixelShaderSSAODesc.EntryPoint = "main";
            _pixelShaderSSAODesc.Language = "hlsl";
            _pixelShaderSSAODesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderSSAODesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/SSAOBlur_VS.hlsl"));
            _vertexShaderSSAOBlurDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderSSAOBlurDesc.FilePath = SHADERS_FOLDER + String("HLSL/SSAOBlur_VS.hlsl");
            _vertexShaderSSAOBlurDesc.EntryPoint = "main";
            _vertexShaderSSAOBlurDesc.Language = "hlsl";
            _vertexShaderSSAOBlurDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderSSAOBlurDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/SSAOBlur_PS.hlsl"));
            _pixelShaderSSAOBlurDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderSSAOBlurDesc.FilePath = SHADERS_FOLDER + String("HLSL/SSAOBlur_PS.hlsl");
            _pixelShaderSSAOBlurDesc.EntryPoint = "main";
            _pixelShaderSSAOBlurDesc.Language = "hlsl";
            _pixelShaderSSAOBlurDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderSSAOBlurDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/SSAODownSample_VS.hlsl"));
            _vertexShaderSSAODownSampleDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderSSAODownSampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/SSAODownSample_VS.hlsl");
            _vertexShaderSSAODownSampleDesc.EntryPoint = "main";
            _vertexShaderSSAODownSampleDesc.Language = "hlsl";
            _vertexShaderSSAODownSampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderSSAODownSampleDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/SSAODownSample_PS.hlsl"));
            _pixelShaderSSAODownSampleDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderSSAODownSampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/SSAODownSample_PS.hlsl");
            _pixelShaderSSAODownSampleDesc.EntryPoint = "main";
            _pixelShaderSSAODownSampleDesc.Language = "hlsl";
            _pixelShaderSSAODownSampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderSSAODownSampleDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Decal_VS.hlsl"));
            _vertexShaderDecalDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderDecalDesc.FilePath = SHADERS_FOLDER + String("HLSL/Decal_VS.hlsl");
            _vertexShaderDecalDesc.EntryPoint = "main";
            _vertexShaderDecalDesc.Language = "hlsl";
            _vertexShaderDecalDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderDecalDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Decal_PS.hlsl"));
            _pixelShaderDecalDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderDecalDesc.FilePath = SHADERS_FOLDER + String("HLSL/Decal_PS.hlsl");
            _pixelShaderDecalDesc.EntryPoint = "main";
            _pixelShaderDecalDesc.Language = "hlsl";
            _pixelShaderDecalDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderDecalDesc.Source = shaderFile.GetAsString();
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
            SHADER_DATA_PARAM_DESC gViewportXDesc("gViewportX", "gViewportX", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gViewOriginDesc("gViewOrigin", "gViewOrigin", GPDT_FLOAT3);
            SHADER_DATA_PARAM_DESC gViewportYDesc("gViewportY", "gViewportY", GPDT_INT1);
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
            SHADER_DATA_PARAM_DESC gCastLightsDesc("gCastLights", "gCastLights", GPDT_INT1);

            SHADER_DATA_PARAM_DESC gTime("gTime", "gTime", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gFrameDeltaDesc("gFrameDelta", "gFrameDelta", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gUseSkyboxMapDesc("gUseSkyboxMap", "gUseSkyboxMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseSkyboxIrradianceMapDesc("gUseSkyboxIrradianceMap", "gUseSkyboxIrradianceMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gSceneLightColorDesc("gSceneLightColor", "gSceneLightColor", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gSkyboxBrightnessDesc("gSkyboxBrightness", "gSkyboxBrightness", GPDT_FLOAT1);

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
            SHADER_DATA_PARAM_DESC gUseIrradianceMap("gUseIrradianceMap", "gUseIrradianceMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseGlobalIllumination("gUseGlobalIllumination", "gUseGlobalIllumination", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gSpecularPower("gSpecularPower", "gSpecularPower", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gSpecularStrength("gSpecularStrength", "gSpecularStrength", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gTransparency("gTransparency", "gTransparency", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gIndexOfRefraction("gIndexOfRefraction", "gIndexOfRefraction", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gRefraction("gRefraction", "gRefraction", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gReflection("gReflection", "gReflection", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gBumpScale("gBumScale", "gBumScale", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gParallaxScale("gParallaxScale", "gParallaxScale", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gAlphaThreshold("gAlphaThreshold", "gAlphaThreshold", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gParallaxSamples("gParallaxSamples", "gParallaxSamples", GPDT_INT1);

            SHADER_OBJECT_PARAM_DESC anisotropicSamplerDesc("TextureSampler", "TextureSampler", GPOT_SAMPLER2D);

            SHADER_OBJECT_PARAM_DESC diffuseMapDesc("DiffuseMap", "DiffuseMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC emissiveMapDesc("EmissiveMap", "EmissiveMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC normalMapDesc("NormalMap", "NormalMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC specularMapDesc("SpecularMap", "SpecularMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC bumpMapDesc("BumpMap", "BumpMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC parallaxMapDesc("ParallaxMap", "ParallaxMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC transparencyMapDesc("TransparencyMap", "TransparencyMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC reflectionMapDesc("ReflectionMap", "ReflectionMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC occlusionMapDesc("OcclusionMap", "OcclusionMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC environmentMapDesc("EnvironmentMap", "EnvironmentMap", GPOT_TEXTURECUBE);
            SHADER_OBJECT_PARAM_DESC irradianceMapDesc("IrradianceMap", "IrradianceMap", GPOT_TEXTURECUBE);

            SHADER_DATA_PARAM_DESC gLightsDesc("gLights", "gLights", GPDT_STRUCT);
            gLightsDesc.ElementSize = sizeof(PerLightData);
            SHADER_DATA_PARAM_DESC gLightsNumberDesc("gLightsNumber", "gLightsNumber", GPDT_INT1);

            _forwardShaderDesc.AddParameter(gViewDirDesc);
            _forwardShaderDesc.AddParameter(gViewportXDesc);
            _forwardShaderDesc.AddParameter(gViewOriginDesc);
            _forwardShaderDesc.AddParameter(gViewportYDesc);
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
            _forwardShaderDesc.AddParameter(gCastLightsDesc);
            
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
            _forwardShaderDesc.AddParameter(gUseIrradianceMap);
            _forwardShaderDesc.AddParameter(gUseGlobalIllumination);
            _forwardShaderDesc.AddParameter(gSpecularPower);
            _forwardShaderDesc.AddParameter(gSpecularStrength);
            _forwardShaderDesc.AddParameter(gTransparency);
            _forwardShaderDesc.AddParameter(gIndexOfRefraction);
            _forwardShaderDesc.AddParameter(gRefraction);
            _forwardShaderDesc.AddParameter(gReflection);
            _forwardShaderDesc.AddParameter(gBumpScale);
            _forwardShaderDesc.AddParameter(gParallaxScale);
            _forwardShaderDesc.AddParameter(gAlphaThreshold);
            _forwardShaderDesc.AddParameter(gParallaxSamples);

            _forwardShaderDesc.AddParameter(gTime);
            _forwardShaderDesc.AddParameter(gFrameDeltaDesc);
            _forwardShaderDesc.AddParameter(gUseSkyboxMapDesc);
            _forwardShaderDesc.AddParameter(gUseSkyboxIrradianceMapDesc);
            _forwardShaderDesc.AddParameter(gSceneLightColorDesc);
            _forwardShaderDesc.AddParameter(gSkyboxBrightnessDesc);

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
            _forwardShaderDesc.AddParameter(irradianceMapDesc);

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
            SHADER_DATA_PARAM_DESC gNumSamplesDesc("gNumSamples", "gNumSamples", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gHorizontalDesc("gHorizontal", "gHorizontal", GPDT_INT1);

            SHADER_OBJECT_PARAM_DESC bilinearSamplerDesc("BilinearSampler", "BilinearSampler", GPOT_SAMPLER2D);

            SHADER_OBJECT_PARAM_DESC sourceMapDesc("SourceMap", "SourceMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC SourceMapMSDesc("SourceMapMS", "SourceMapMS", GPOT_RWTEXTURE2DMS);
            
            _gaussianBlurShaderDesc.AddParameter(bilinearSamplerDesc);

            _gaussianBlurShaderDesc.AddParameter(sourceMapDesc);
            _gaussianBlurShaderDesc.AddParameter(SourceMapMSDesc);

            _gaussianBlurShaderDesc.AddParameter(gSourceDimensionsDesc);
            _gaussianBlurShaderDesc.AddParameter(gMSAACountDesc);
            _gaussianBlurShaderDesc.AddParameter(gNumSamplesDesc);
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
            _pickSelectShaderDesc.AddParameter(gMatViewOriginDesc);
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
            gInstanceData.ElementSize = sizeof(PerHudInstanceData);

            SHADER_OBJECT_PARAM_DESC anisotropicSamplerDesc("TextureSampler", "TextureSampler", GPOT_SAMPLER2D);
            SHADER_OBJECT_PARAM_DESC maskTextureDesc("MaskTexture", "MaskTexture", GPOT_TEXTURE2D);

            _hudPickSelectShaderDesc.AddParameter(gMatViewProjDesc);
            _hudPickSelectShaderDesc.AddParameter(gMatViewOriginDesc);
            _hudPickSelectShaderDesc.AddParameter(gRenderTypeDesc);

            _hudPickSelectShaderDesc.AddParameter(gInstanceData);

            _hudPickSelectShaderDesc.AddParameter(anisotropicSamplerDesc);
            _hudPickSelectShaderDesc.AddParameter(maskTextureDesc);
        }

        {
            SHADER_DATA_PARAM_DESC gMatViewProjDesc("gMatViewProj", "gMatViewProj", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatViewOriginDesc("gViewOrigin", "gViewOrigin", GPDT_FLOAT3);

            SHADER_DATA_PARAM_DESC gInstanceData("gInstanceData", "gInstanceData", GPDT_STRUCT);
            gInstanceData.ElementSize = sizeof(PerBulletDebugInstanceData);

            _bulletDebugShaderDesc.AddParameter(gMatViewProjDesc);
            _bulletDebugShaderDesc.AddParameter(gMatViewOriginDesc);

            _bulletDebugShaderDesc.AddParameter(gInstanceData);
        }

        {
            // TODO SSAO
        }

        {
            // TODO SSAO BLUR
        }

        {
            // TODO SSAO DOWN SAMPLE
        }

        {
            // TODO DECAL
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

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _forwardShaderDesc;
        shaderDesc.QueueType = QueueSortType::FrontToBack;
        shaderDesc.Techniques.push_back(technique);

        _shaderOpaque = Shader::Create("ForwardOpaque", shaderDesc);
    }

    void BuiltinResources::InitShaderTransparent(bool cull)
    {
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendEnable = true;
        _blendTransparentStateDesc.RenderTargetDesc[0].SrcBlend = BlendFactor::BF_SOURCE_ALPHA;
        _blendTransparentStateDesc.RenderTargetDesc[0].DstBlend = BlendFactor::BF_INV_SOURCE_ALPHA;
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendOp = BlendOperation::BO_ADD;
        _blendTransparentStateDesc.RenderTargetDesc[0].SrcBlendAlpha = BlendFactor::BF_ONE;
        _blendTransparentStateDesc.RenderTargetDesc[0].DstBlendAlpha = BlendFactor::BF_ZERO;
        _blendTransparentStateDesc.RenderTargetDesc[0].BlendOpAlpha = BlendOperation::BO_ADD;
        _blendTransparentStateDesc.RenderTargetDesc[0].RenderTargetWriteMask = CO_ENABLE_ALL;

        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendTransparentStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderForwardDesc;
        passDesc.PixelProgramDesc = _pixelShaderForwardDesc;

        if(cull)
            passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_CLOCKWISE;
        else
            passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _forwardShaderDesc;
        shaderDesc.Flags = (UINT32)ShaderFlag::Transparent;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        if(cull)
            _shaderTransparent = Shader::Create("ForwardTransparent", shaderDesc);
        else
            _shaderTransparentCullNone = Shader::Create("ForwardTransparentCullNone", shaderDesc);
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

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _blitShaderDesc;
        shaderDesc.Techniques.push_back(technique);

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

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _skyboxShaderDesc;
        shaderDesc.Techniques.push_back(technique);

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

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _FXAAShaderDesc;
        shaderDesc.Techniques.push_back(technique);

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

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _toneMappingShaderDesc;
        shaderDesc.Techniques.push_back(technique);

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

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _bloomShaderDesc;
        shaderDesc.Techniques.push_back(technique);

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

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _motionBlurShaderDesc;
        shaderDesc.Techniques.push_back(technique);

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

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _motionBlurShaderDesc;
        shaderDesc.Techniques.push_back(technique);

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

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _pickSelectShaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderPicking = Shader::Create("Picking", shaderDesc);
    }

    void BuiltinResources::InitShaderSelection()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderPickSelectDesc;
        passDesc.PixelProgramDesc = _pixelShaderPickSelectDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_CLOCKWISE;
        passDesc.RasterizerStateDesc.polygonMode = PolygonMode::PM_WIREFRAME;
        passDesc.RasterizerStateDesc.depthBias = 0.00001f;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _pickSelectShaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

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

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _hudPickSelectShaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderHudPicking = Shader::Create("HudPicking", shaderDesc);
    }

    void BuiltinResources::InitShaderHudSelection()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderHudPickSelectDesc;
        passDesc.GeometryProgramDesc = _geometryShaderHudPickSelectDesc;
        passDesc.PixelProgramDesc = _pixelShaderHudPickSelectDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;
        passDesc.RasterizerStateDesc.polygonMode = PolygonMode::PM_WIREFRAME;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _hudPickSelectShaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderHudSelection = Shader::Create("HudSelection", shaderDesc);
    }

    void BuiltinResources::InitShaderBulletDebug()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderBulletDebugDesc;
        passDesc.GeometryProgramDesc = _geometryShaderBulletDebugDesc;
        passDesc.PixelProgramDesc = _pixelShaderBulletDebugDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _hudPickSelectShaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderBulletDebug = Shader::Create("BulletDebug", shaderDesc);
    }

    void BuiltinResources::InitShaderSSAO()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderSSAODesc;
        passDesc.PixelProgramDesc = _pixelShaderSSAODesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _ssaoShaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderSSAO = Shader::Create("SSAO", shaderDesc);
    }

    void BuiltinResources::InitShaderSSAOBlur()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderSSAOBlurDesc;
        passDesc.PixelProgramDesc = _pixelShaderSSAOBlurDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _ssaoBlurShaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderSSAO = Shader::Create("SSAO Blur", shaderDesc);
    }

    void BuiltinResources::InitShaderSSAODownSample()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderSSAODownSampleDesc;
        passDesc.PixelProgramDesc = _pixelShaderSSAODownSampleDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _ssaoDownSampleShaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderSSAO = Shader::Create("SSAO Down Sample", shaderDesc);
    }

    void BuiltinResources::InitShaderDecal()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderDecalDesc;
        passDesc.PixelProgramDesc = _pixelShaderDecalDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _decalShaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderDecal = Shader::Create("Decal", shaderDesc);
    }

    void BuiltinResources::InitDefaultMaterial()
    {
        MaterialProperties properties;
        _defaultMaterial = Material::Create(GetBuiltinShader(BuiltinShader::Opaque));
        _defaultMaterial->SetProperties(properties);
    }

    void BuiltinResources::InitFrameworkIcon()
    {
        SPtr<TextureImportOptions> options = te_shared_ptr_new<TextureImportOptions>();
        options->GenerateMips = false;

        HTexture iconTex = gResourceManager().Load<Texture>(ICONS_FOLDER + String("frameworkIcon.png"), options);

        _frameworkIcon = iconTex->GetProperties().AllocBuffer(0, 0);
        iconTex->ReadData(*_frameworkIcon.get());
    }

    BuiltinResources& gBuiltinResources()
    {
        return BuiltinResources::Instance();
    }
}

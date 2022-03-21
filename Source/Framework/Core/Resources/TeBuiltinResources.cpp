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
        _noFilterSamplerState = nullptr;
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
        case BuiltinShader::ReflectionCubeDownsample:
            if (!_shaderReflectionCubeDownsample.IsLoaded())
                InitShaderReflectionCubeDownsample();
            shader = _shaderReflectionCubeDownsample;
            break;
        case BuiltinShader::ReflectionCubeImportanceSample:
            if (!_shaderReflectionCubeImportanceSample.IsLoaded())
                InitShaderReflectionCubeImportanceSample();
            shader = _shaderReflectionCubeImportanceSample;
            break;
        case BuiltinShader::IrradianceComputeSH:
            if (!_shaderIrradianceComputeSH.IsLoaded())
                InitIrradianceComputeSH();
            shader = _shaderIrradianceComputeSH;
            break;
        case BuiltinShader::IrradianceReduceSH:
            if (!_shaderIrradianceReduceSH.IsLoaded())
                InitIrradianceReduceSH();
            shader = _shaderIrradianceReduceSH;
            break;
        case BuiltinShader::IrradianceProjectSH:
            if (!_shaderIrradianceProjectSH.IsLoaded())
                InitIrradianceProjectSH();
            shader = _shaderIrradianceProjectSH;
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
        case BuiltinSampler::NoFilter:
            return _noFilterSamplerState; 
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
            _vertexShaderBlitDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderBlitDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Blit_PS.hlsl"));
            _pixelShaderBlitDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderBlitDesc.FilePath = SHADERS_FOLDER + String("HLSL/Blit_PS.hlsl");
            _pixelShaderBlitDesc.EntryPoint = "main";
            _pixelShaderBlitDesc.Language = "hlsl";
            _pixelShaderBlitDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderBlitDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Skybox_VS.hlsl"));
            _vertexShaderSkyboxDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderSkyboxDesc.FilePath = SHADERS_FOLDER + String("HLSL/Skybox_VS.hlsl");
            _vertexShaderSkyboxDesc.EntryPoint = "main";
            _vertexShaderSkyboxDesc.Language = "hlsl";
            _vertexShaderSkyboxDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderSkyboxDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/Skybox_PS.hlsl"));
            _pixelShaderSkyboxDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderSkyboxDesc.FilePath = SHADERS_FOLDER + String("HLSL/Skybox_PS.hlsl");
            _pixelShaderSkyboxDesc.EntryPoint = "main";
            _pixelShaderSkyboxDesc.Language = "hlsl";
            _pixelShaderSkyboxDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
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

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ReflectionCubeDownsample_VS.hlsl"));
            _vertexShaderReflectionCubeDownsampleDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderReflectionCubeDownsampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/ReflectionCubeDownsample_VS.hlsl");
            _vertexShaderReflectionCubeDownsampleDesc.EntryPoint = "main";
            _vertexShaderReflectionCubeDownsampleDesc.Language = "hlsl";
            _vertexShaderReflectionCubeDownsampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderReflectionCubeDownsampleDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ReflectionCubeDownsample_PS.hlsl"));
            _pixelShaderReflectionCubeDownsampleDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderReflectionCubeDownsampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/ReflectionCubeDownsample_PS.hlsl");
            _pixelShaderReflectionCubeDownsampleDesc.EntryPoint = "main";
            _pixelShaderReflectionCubeDownsampleDesc.Language = "hlsl";
            _pixelShaderReflectionCubeDownsampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderReflectionCubeDownsampleDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ReflectionCubeImportanceSample_VS.hlsl"));
            _vertexShaderReflectionCubeImportanceSampleDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderReflectionCubeImportanceSampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/ReflectionCubeImportanceSample_VS.hlsl");
            _vertexShaderReflectionCubeImportanceSampleDesc.EntryPoint = "main";
            _vertexShaderReflectionCubeImportanceSampleDesc.Language = "hlsl";
            _vertexShaderReflectionCubeImportanceSampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderReflectionCubeImportanceSampleDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ReflectionCubeImportanceSample_PS.hlsl"));
            _pixelShaderReflectionCubeImportanceSampleDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderReflectionCubeImportanceSampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/ReflectionCubeImportanceSample_PS.hlsl");
            _pixelShaderReflectionCubeImportanceSampleDesc.EntryPoint = "main";
            _pixelShaderReflectionCubeImportanceSampleDesc.Language = "hlsl";
            _pixelShaderReflectionCubeImportanceSampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderReflectionCubeImportanceSampleDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/IrradianceComputeSH_CS.hlsl"));
            _computeShaderIrradianceComputeSHDesc.Type = GPT_COMPUTE_PROGRAM;
            _computeShaderIrradianceComputeSHDesc.FilePath = SHADERS_FOLDER + String("HLSL/IrradianceComputeSH_CS.hlsl");
            _computeShaderIrradianceComputeSHDesc.EntryPoint = "main";
            _computeShaderIrradianceComputeSHDesc.Language = "hlsl";
            _computeShaderIrradianceComputeSHDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _computeShaderIrradianceComputeSHDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/IrradianceReduceSH_CS.hlsl"));
            _computeShaderIrradianceReduceSHDesc.Type = GPT_COMPUTE_PROGRAM;
            _computeShaderIrradianceReduceSHDesc.FilePath = SHADERS_FOLDER + String("HLSL/IrradianceReduceSH_CS.hlsl");
            _computeShaderIrradianceReduceSHDesc.EntryPoint = "main";
            _computeShaderIrradianceReduceSHDesc.Language = "hlsl";
            _computeShaderIrradianceReduceSHDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _computeShaderIrradianceReduceSHDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/IrradianceProjectSH_VS.hlsl"));
            _vertexShaderIrradianceProjectSHDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderIrradianceProjectSHDesc.FilePath = SHADERS_FOLDER + String("HLSL/IrradianceProjectSH_VS.hlsl");
            _vertexShaderIrradianceProjectSHDesc.EntryPoint = "main";
            _vertexShaderIrradianceProjectSHDesc.Language = "hlsl";
            _vertexShaderIrradianceProjectSHDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderIrradianceProjectSHDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/IrradianceProjectSH_PS.hlsl"));
            _pixelShaderIrradianceProjectSHDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderIrradianceProjectSHDesc.FilePath = SHADERS_FOLDER + String("HLSL/IrradianceProjectSH_PS.hlsl");
            _pixelShaderIrradianceProjectSHDesc.EntryPoint = "main";
            _pixelShaderIrradianceProjectSHDesc.Language = "hlsl";
            _pixelShaderIrradianceProjectSHDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderIrradianceProjectSHDesc.Source = shaderFile.GetAsString();
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

        _noFilterSamplerStateDesc.AddressMode = UVWAddressingMode();
        _noFilterSamplerStateDesc.MinFilter = FO_NONE;
        _noFilterSamplerStateDesc.MagFilter = FO_NONE;
        _noFilterSamplerStateDesc.MipFilter = FO_NONE;
    }

    void BuiltinResources::InitShaderDesc()
    { }

    void BuiltinResources::InitSamplers()
    {
        _anisotropicSamplerState = SamplerState::Create(_anisotropicSamplerStateDesc);
        _noFilterSamplerState = SamplerState::Create(_noFilterSamplerStateDesc);
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

        SHADER_DESC shaderDesc = _bulletDebugShaderDesc;
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

    void BuiltinResources::InitShaderReflectionCubeDownsample()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderReflectionCubeDownsampleDesc;
        passDesc.PixelProgramDesc = _pixelShaderReflectionCubeDownsampleDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _shaderReflectionCubeDownsampleDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderReflectionCubeDownsample = Shader::Create("Reflection Cube Down Sample", shaderDesc);
    }

    void BuiltinResources::InitShaderReflectionCubeImportanceSample()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderReflectionCubeImportanceSampleDesc;
        passDesc.PixelProgramDesc = _pixelShaderReflectionCubeImportanceSampleDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _shaderReflectionCubeImportanceSampleDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderReflectionCubeImportanceSample = Shader::Create("Reflection Cube Importance Sample", shaderDesc);
    }

    void BuiltinResources::InitIrradianceComputeSH()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.ComputeProgramDesc = _computeShaderIrradianceComputeSHDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _shaderIrradianceComputeSHDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderIrradianceComputeSH = Shader::Create("Irradiance Compute SH", shaderDesc);
    }

    void BuiltinResources::InitIrradianceReduceSH()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.ComputeProgramDesc = _computeShaderIrradianceReduceSHDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _shaderIrradianceReduceSHDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderIrradianceReduceSH = Shader::Create("Irradiance Reduce SH", shaderDesc);
    }

    void BuiltinResources::InitIrradianceProjectSH()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderIrradianceProjectSHDesc;
        passDesc.PixelProgramDesc = _pixelShaderIrradianceProjectSHDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc = _shaderIrradianceProjectSHDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderIrradianceProjectSH = Shader::Create("Irradiance Project SH", shaderDesc);
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

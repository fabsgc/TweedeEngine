#include "Resources/TeBuiltinResources.h"
#include "Resources/TeResourceManager.h"
#include "Image/TeTexture.h"
#include "Material/TePass.h"
#include "Material/TeMaterial.h"
#include "Material/TeTechnique.h"
#include "Utility/TeDataStream.h"
#include "Importer/TeTextureImportOptions.h"
#include "Utility/TePoolAllocator.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(BuiltinResources)

        IMPLEMENT_GLOBAL_POOL(ShaderVariationParam, 64)

    BuiltinResources::BuiltinResources()
    { }

    BuiltinResources::~BuiltinResources()
    { 
        _anisotropicSamplerState = nullptr;
        _noFilterSamplerState = nullptr;
        _bilinearSamplerState = nullptr;
        _bilinearClampedSamplerState = nullptr;
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
        case BuiltinShader::ZPrepass:
            if (!_shaderZPrepass.IsLoaded())
                InitShaderZPrepass();
            shader = _shaderZPrepass;
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
        case BuiltinShader::BlitSelection:
            if (!_shaderBlitSelection.IsLoaded())
                InitShaderBlitSelection();
            shader = _shaderBlitSelection;
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
        case BuiltinShader::TextureDownsample:
            if (!_shaderTextureDownsample.IsLoaded())
                InitShaderTextureDownsample();
            shader = _shaderTextureDownsample;
            break;
        case BuiltinShader::TextureCubeDownsample:
            if (!_shaderTextureCubeDownsample.IsLoaded())
                InitShaderTextureCubeDownsample();
            shader = _shaderTextureCubeDownsample;
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
        case BuiltinShader::ShadowDepthNormal:
            if (!_shaderShadowDepthNormal.IsLoaded())
                InitShaderShadowDepthNormal();
            shader = _shaderShadowDepthNormal;
            break;
        case BuiltinShader::ShadowDepthCube:
            if (!_shaderShadowDepthCube.IsLoaded())
                InitShaderShadowDepthCube();
            shader = _shaderShadowDepthCube;
            break;
        case BuiltinShader::ShadowDepthDirectional:
            if (!_shaderShadowDepthDirectional.IsLoaded())
                InitShaderShadowDepthDirectional();
            shader = _shaderShadowDepthDirectional;
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
        case BuiltinSampler::Trilinear:
            return _trilinearSamplerState;
        case BuiltinSampler::Bilinear:
            return _bilinearSamplerState;
        case BuiltinSampler::BilinearClamped:
            return _bilinearClampedSamplerState;
        case BuiltinSampler::NearestPoint:
            return _nearestPointSamplerState;
        case BuiltinSampler::NearestPointClamped:
            return _nearestPointClampedSamplerState;
        case BuiltinSampler::NoFilter:
            return _noFilterSamplerState; 
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
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ForwardZPrepass_VS.hlsl"));
            _vertexShaderForwardZPrepassDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderForwardZPrepassDesc.FilePath = SHADERS_FOLDER + String("HLSL/ForwardZPrepass_VS.hlsl");
            _vertexShaderForwardZPrepassDesc.EntryPoint = "main";
            _vertexShaderForwardZPrepassDesc.Language = "hlsl";
            _vertexShaderForwardZPrepassDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderForwardZPrepassDesc.Source = shaderFile.GetAsString();
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
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/BlitSelect_VS.hlsl"));
            _vertexShaderBlitSelectDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderBlitSelectDesc.FilePath = SHADERS_FOLDER + String("HLSL/BlitSelect_VS.hlsl");
            _vertexShaderBlitSelectDesc.EntryPoint = "main";
            _vertexShaderBlitSelectDesc.Language = "hlsl";
            _vertexShaderBlitSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderBlitSelectDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/BlitSelect_PS.hlsl"));
            _pixelShaderBlitSelectDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderBlitSelectDesc.FilePath = SHADERS_FOLDER + String("HLSL/BlitSelect_PS.hlsl");
            _pixelShaderBlitSelectDesc.EntryPoint = "main";
            _pixelShaderBlitSelectDesc.Language = "hlsl";
            _pixelShaderBlitSelectDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderBlitSelectDesc.Source = shaderFile.GetAsString();
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
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/SSAODownsample_VS.hlsl"));
            _vertexShaderSSAODownSampleDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderSSAODownSampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/SSAODownsample_VS.hlsl");
            _vertexShaderSSAODownSampleDesc.EntryPoint = "main";
            _vertexShaderSSAODownSampleDesc.Language = "hlsl";
            _vertexShaderSSAODownSampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderSSAODownSampleDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/SSAODownsample_PS.hlsl"));
            _pixelShaderSSAODownSampleDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderSSAODownSampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/SSAODownsample_PS.hlsl");
            _pixelShaderSSAODownSampleDesc.EntryPoint = "main";
            _pixelShaderSSAODownSampleDesc.Language = "hlsl";
            _pixelShaderSSAODownSampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderSSAODownSampleDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/TextureDownsample_VS.hlsl"));
            _vertexShaderTextureDownsampleDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderTextureDownsampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/TextureDownsample_VS.hlsl");
            _vertexShaderTextureDownsampleDesc.EntryPoint = "main";
            _vertexShaderTextureDownsampleDesc.Language = "hlsl";
            _vertexShaderTextureDownsampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderTextureDownsampleDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/TextureDownsample_PS.hlsl"));
            _pixelShaderTextureDownsampleDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderTextureDownsampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/TextureDownsample_PS.hlsl");
            _pixelShaderTextureDownsampleDesc.EntryPoint = "main";
            _pixelShaderTextureDownsampleDesc.Language = "hlsl";
            _pixelShaderTextureDownsampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderTextureDownsampleDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/TextureCubeDownsample_VS.hlsl"));
            _vertexShaderTextureCubeDownsampleDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderTextureCubeDownsampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/TextureCubeDownsample_VS.hlsl");
            _vertexShaderTextureCubeDownsampleDesc.EntryPoint = "main";
            _vertexShaderTextureCubeDownsampleDesc.Language = "hlsl";
            _vertexShaderTextureCubeDownsampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderTextureCubeDownsampleDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/TextureCubeDownsample_PS.hlsl"));
            _pixelShaderTextureCubeDownsampleDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderTextureCubeDownsampleDesc.FilePath = SHADERS_FOLDER + String("HLSL/TextureCubeDownsample_PS.hlsl");
            _pixelShaderTextureCubeDownsampleDesc.EntryPoint = "main";
            _pixelShaderTextureCubeDownsampleDesc.Language = "hlsl";
            _pixelShaderTextureCubeDownsampleDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderTextureCubeDownsampleDesc.Source = shaderFile.GetAsString();
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
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ShadowDepthNormal_VS.hlsl"));
            _vertexShaderShadowDepthNormalDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderShadowDepthNormalDesc.FilePath = SHADERS_FOLDER + String("HLSL/ShadowDepthNormal_VS.hlsl");
            _vertexShaderShadowDepthNormalDesc.EntryPoint = "main";
            _vertexShaderShadowDepthNormalDesc.Language = "hlsl";
            _vertexShaderShadowDepthNormalDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderShadowDepthNormalDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ShadowDepthNormal_PS.hlsl"));
            _pixelShaderShadowDepthNormalDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderShadowDepthNormalDesc.FilePath = SHADERS_FOLDER + String("HLSL/ShadowDepthNormal_PS.hlsl");
            _pixelShaderShadowDepthNormalDesc.EntryPoint = "main";
            _pixelShaderShadowDepthNormalDesc.Language = "hlsl";
            _pixelShaderShadowDepthNormalDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderShadowDepthNormalDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ShadowDepthCube_VS.hlsl"));
            _vertexShaderShadowDepthCubeDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderShadowDepthCubeDesc.FilePath = SHADERS_FOLDER + String("HLSL/ShadowDepthCube_VS.hlsl");
            _vertexShaderShadowDepthCubeDesc.EntryPoint = "main";
            _vertexShaderShadowDepthCubeDesc.Language = "hlsl";
            _vertexShaderShadowDepthCubeDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderShadowDepthCubeDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ShadowDepthCube_PS.hlsl"));
            _pixelShaderShadowDepthCubeDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderShadowDepthCubeDesc.FilePath = SHADERS_FOLDER + String("HLSL/ShadowDepthCube_PS.hlsl");
            _pixelShaderShadowDepthCubeDesc.EntryPoint = "main";
            _pixelShaderShadowDepthCubeDesc.Language = "hlsl";
            _pixelShaderShadowDepthCubeDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderShadowDepthCubeDesc.Source = shaderFile.GetAsString();
        }
        
        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ShadowDepthDirectional_VS.hlsl"));
            _vertexShaderShadowDepthDirectionalDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderShadowDepthDirectionalDesc.FilePath = SHADERS_FOLDER + String("HLSL/ShadowDepthDirectional_VS.hlsl");
            _vertexShaderShadowDepthDirectionalDesc.EntryPoint = "main";
            _vertexShaderShadowDepthDirectionalDesc.Language = "hlsl";
            _vertexShaderShadowDepthDirectionalDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _vertexShaderShadowDepthDirectionalDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("HLSL/ShadowDepthDirectional_PS.hlsl"));
            _pixelShaderShadowDepthDirectionalDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderShadowDepthDirectionalDesc.FilePath = SHADERS_FOLDER + String("HLSL/ShadowDepthDirectional_PS.hlsl");
            _pixelShaderShadowDepthDirectionalDesc.EntryPoint = "main";
            _pixelShaderShadowDepthDirectionalDesc.Language = "hlsl";
            _pixelShaderShadowDepthDirectionalDesc.IncludePath = SHADERS_FOLDER + String("HLSL/");
            _pixelShaderShadowDepthDirectionalDesc.Source = shaderFile.GetAsString();
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

        _trilinearSamplerStateDesc.AddressMode = UVWAddressingMode();
        _trilinearSamplerStateDesc.MinFilter = FO_LINEAR;
        _trilinearSamplerStateDesc.MagFilter = FO_LINEAR;
        _trilinearSamplerStateDesc.MipFilter = FO_LINEAR;

        _bilinearSamplerStateDesc.AddressMode = UVWAddressingMode();
        _bilinearSamplerStateDesc.MinFilter = FO_LINEAR;
        _bilinearSamplerStateDesc.MagFilter = FO_LINEAR;
        _bilinearSamplerStateDesc.MipFilter = FO_POINT;

        _bilinearClampedSamplerStateDesc.AddressMode.u = TextureAddressingMode::TAM_CLAMP;
        _bilinearClampedSamplerStateDesc.AddressMode.v = TextureAddressingMode::TAM_CLAMP;
        _bilinearClampedSamplerStateDesc.AddressMode.w = TextureAddressingMode::TAM_CLAMP;
        _bilinearClampedSamplerStateDesc.MinFilter = FO_LINEAR;
        _bilinearClampedSamplerStateDesc.MagFilter = FO_LINEAR;
        _bilinearClampedSamplerStateDesc.MipFilter = FO_POINT;

        _nearestPointSamplerStateDesc.AddressMode = UVWAddressingMode();
        _nearestPointSamplerStateDesc.MinFilter = FO_POINT;
        _nearestPointSamplerStateDesc.MagFilter = FO_POINT;
        _nearestPointSamplerStateDesc.MipFilter = FO_POINT;

        _nearestPointClampedSamplerStateDesc.AddressMode.u = TextureAddressingMode::TAM_CLAMP;
        _nearestPointClampedSamplerStateDesc.AddressMode.v = TextureAddressingMode::TAM_CLAMP;
        _nearestPointClampedSamplerStateDesc.AddressMode.w = TextureAddressingMode::TAM_CLAMP;
        _nearestPointClampedSamplerStateDesc.MinFilter = FO_POINT;
        _nearestPointClampedSamplerStateDesc.MagFilter = FO_POINT;
        _nearestPointClampedSamplerStateDesc.MipFilter = FO_POINT;

        _noFilterSamplerStateDesc.AddressMode = UVWAddressingMode();
        _noFilterSamplerStateDesc.MinFilter = FO_NONE;
        _noFilterSamplerStateDesc.MagFilter = FO_NONE;
        _noFilterSamplerStateDesc.MipFilter = FO_NONE;

        _noFilterClampedSamplerStateDesc.AddressMode.u = TextureAddressingMode::TAM_CLAMP;
        _noFilterClampedSamplerStateDesc.AddressMode.v = TextureAddressingMode::TAM_CLAMP;
        _noFilterClampedSamplerStateDesc.AddressMode.w = TextureAddressingMode::TAM_CLAMP;
        _noFilterClampedSamplerStateDesc.MinFilter = FO_NONE;
        _noFilterClampedSamplerStateDesc.MagFilter = FO_NONE;
        _noFilterClampedSamplerStateDesc.MipFilter = FO_NONE;
    }

    void BuiltinResources::InitShaderDesc()
    { }

    void BuiltinResources::InitSamplers()
    {
        _anisotropicSamplerState = SamplerState::Create(_anisotropicSamplerStateDesc);
        _trilinearSamplerState = SamplerState::Create(_trilinearSamplerStateDesc);
        _bilinearSamplerState = SamplerState::Create(_bilinearSamplerStateDesc);
        _bilinearClampedSamplerState = SamplerState::Create(_bilinearClampedSamplerStateDesc);
        _nearestPointSamplerState = SamplerState::Create(_nearestPointSamplerStateDesc);
        _nearestPointClampedSamplerState = SamplerState::Create(_nearestPointClampedSamplerStateDesc);
        _noFilterSamplerState = SamplerState::Create(_noFilterSamplerStateDesc);
        _noFilterClampedSamplerState = SamplerState::Create(_noFilterClampedSamplerStateDesc);
    }

    List<ShaderVariation> BuiltinResources::FillShaderVariations(const Vector<ShaderVariationParam*>& iShaderVariationParamsList)
    {
        List<ShaderVariation> variations;
        UINT32 numberOfVariations = 1;

        auto CartesianProduct = [](const ShaderVariationParam& left, const ShaderVariationParam& right)
        {
            size_t valuesNumber = left.second.size() * right.second.size();
            ShaderVariationParam* resultLeft = te_pool_new<ShaderVariationParam>();
            ShaderVariationParam* resultRight = te_pool_new<ShaderVariationParam>();

            resultLeft->second.resize(valuesNumber);
            resultRight->second.resize(valuesNumber);

            resultLeft->first = left.first;
            resultRight->first = right.first;

            UINT32 i = 0;

            for (auto& rightValue : right.second)
            {
                for (auto& leftValue : left.second)
                {
                    resultLeft->second[i] = leftValue;
                    resultRight->second[i] = rightValue;

                    i++;
                }
            }

            return Vector<ShaderVariationParam*> { resultLeft, resultRight };
        };

        auto CreateVariation = [](const String& name, const std::any& value, ShaderVariation& oVariation)
        {
            if (value.type() == typeid(float))
                oVariation.AddParam(ShaderVariation::Param(name, std::any_cast<float>(value)));
            else if (value.type() == typeid(UINT32))
                oVariation.AddParam(ShaderVariation::Param(name, std::any_cast<UINT32>(value)));
            else if (value.type() == typeid(INT32))
                oVariation.AddParam(ShaderVariation::Param(name, std::any_cast<INT32>(value)));
            else if (value.type() == typeid(bool))
                oVariation.AddParam(ShaderVariation::Param(name, std::any_cast<bool>(value)));
        };

        for (auto& shaderVariationDesc : iShaderVariationParamsList)
            numberOfVariations *= static_cast<UINT32>(shaderVariationDesc->second.size());

        if (numberOfVariations > 16)
        {
            for (auto& shaderVariationDesc : iShaderVariationParamsList)
                te_pool_delete(shaderVariationDesc);

            return variations;
        }

        if (iShaderVariationParamsList.size() > 1)
        {
            // Create cartesian product of two first ShaderVariationParam
            Vector<ShaderVariationParam*> cartesianProduct = CartesianProduct(*iShaderVariationParamsList[0], *iShaderVariationParamsList[1]);

            // Create cartesian product of first then second ShaderVariationParam with n th ShaderVariationParam
            if (iShaderVariationParamsList.size() > 2)
            {
                for (UINT32 i = 2; i < iShaderVariationParamsList.size(); i++)
                {
                    Vector<ShaderVariationParam*> tmp1;
                    Vector<ShaderVariationParam*> tmp2;

                    for (UINT32 j = 0; j < cartesianProduct.size(); j++)
                    {
                        tmp2 = CartesianProduct(*cartesianProduct[j], *iShaderVariationParamsList[i]);
                        tmp1.push_back(tmp2[0]);

                        if (j == cartesianProduct.size() - 1) { tmp1.push_back(tmp2[1]); }
                        else { te_pool_delete(tmp2[1]); }
                    }

                    for (auto& variationParam : cartesianProduct)
                        te_pool_delete(variationParam);

                    cartesianProduct = tmp1;
                }
            }

            // Create all final variations
            for (UINT32 i = 0; i < numberOfVariations; i++)
            {
                ShaderVariation variation;

                for (auto& variationParam : cartesianProduct)
                {
                    if (variationParam->second.size() > i && variationParam->second[i].has_value())
                        CreateVariation(variationParam->first, variationParam->second[i], variation);
                }

                variations.push_back(variation);
            }

            for (auto& variationParam : cartesianProduct)
                te_pool_delete(variationParam);
        }
        else
        {
            for (auto& value : iShaderVariationParamsList[0]->second)
            {
                if (value.has_value())
                {
                    ShaderVariation variation;
                    CreateVariation(iShaderVariationParamsList[0]->first, value, variation);

                    if (variation.HasParam(iShaderVariationParamsList[0]->first))
                        variations.push_back(variation);

                }
            }
        }

        for (auto& value : iShaderVariationParamsList)
            te_pool_delete(value);

        te_pool_prune<ShaderVariationParam>();

        return variations;
    }

    void BuiltinResources::FillShaderDesc(const Vector<ShaderVariationParam*>& iVariationParams, SHADER_DESC& ioShaderDesc)
    {
        for (auto& variationParam : iVariationParams)
        {
            ShaderVariationParamInfo variationParamInfo(variationParam->first, {});

            for (auto& value : variationParam->second)
            {
                if (value.type() == typeid(float))
                    variationParamInfo.Values.push_back(ShaderVariation::Param(variationParamInfo.Name, std::any_cast<float>(value)));
                else if (value.type() == typeid(UINT32))
                    variationParamInfo.Values.push_back(ShaderVariation::Param(variationParamInfo.Name, std::any_cast<UINT32>(value)));
                else if (value.type() == typeid(INT32))
                    variationParamInfo.Values.push_back(ShaderVariation::Param(variationParamInfo.Name, std::any_cast<INT32>(value)));
                else if (value.type() == typeid(bool))
                    variationParamInfo.Values.push_back(ShaderVariation::Param(variationParamInfo.Name, std::any_cast<bool>(value)));
            }

            ioShaderDesc.VariationParams.push_back(variationParamInfo);
        }
    }

    HShader BuiltinResources::InitShader(List<ShaderVariation>& variations, SHADER_DESC& shaderDesc, 
            const PASS_DESC& passDesc, const String& name, bool defaultShader)
    {
        for (auto& variation : variations)
        {
            SPtr<Pass> pass = Pass::Create(passDesc);
            SPtr<Technique> technique = Technique::Create("hlsl", {}, variation, { pass });
            shaderDesc.Techniques.push_back(technique);
        }

        if (defaultShader || variations.size() == 0)
        {
            SPtr<Pass> pass = Pass::Create(passDesc);
            SPtr<Technique> technique = Technique::Create("hlsl", { pass });
            shaderDesc.Techniques.push_back(technique);
        }

        return Shader::Create(name, shaderDesc);
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

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::FrontToBack;

        Vector<ShaderVariationParam*> variationParams = {
            te_pool_new<ShaderVariationParam>("WRITE_VELOCITY", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("SKINNED", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("TRANSPARENT", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_BASE_COLOR_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_ROUGHNESS_MAP", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("USE_METALLIC_ROUGHNESS_MAP", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("USE_REFLECTANCE_MAP", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("USE_OCCLUSION_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_EMISSIVE_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_SHEEN_COLOR_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_SHEEN_ROUGHNESS_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_CLEAR_COAT_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_CLEAR_COAT_ROUGHNESS_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_CLEAR_COAT_NORMAL_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_NORMAL_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_PARALLAX_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_TRANSMISSION_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_OPACITY_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_ANISOTROPY_DIRECTION_MAP", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("DO_INDIRECT_LIGHTING", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("DO_DIRECT_LIGHTING", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("USE_SHEEN", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("USE_CLEAR_COAT", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("USE_ANISOTROPY", std::forward<Vector<std::any>>({false, true}))
        };

        FillShaderDesc(variationParams, shaderDesc);
        List<ShaderVariation> variations = FillShaderVariations(variationParams);

        _shaderOpaque = InitShader(variations, shaderDesc, passDesc, "Forward Opaque");
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

        if(cull) passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_CLOCKWISE;
        else passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SHADER_DESC shaderDesc;
        shaderDesc.Flags = (UINT32)ShaderFlag::Transparent;
        shaderDesc.QueueType = QueueSortType::BackToFront;

        Vector<ShaderVariationParam*> variationParams = {
            te_pool_new<ShaderVariationParam>("WRITE_VELOCITY", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("SKINNED", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("TRANSPARENT", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_BASE_COLOR_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_ROUGHNESS_MAP", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("USE_METALLIC_ROUGHNESS_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_REFLECTANCE_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_OCCLUSION_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_EMISSIVE_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_SHEEN_COLOR_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_SHEEN_ROUGHNESS_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_CLEAR_COAT_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_CLEAR_COAT_ROUGHNESS_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_CLEAR_COAT_NORMAL_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_NORMAL_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_PARALLAX_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_TRANSMISSION_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_OPACITY_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("USE_ANISOTROPY_DIRECTION_MAP", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("DO_INDIRECT_LIGHTING", std::forward<Vector<std::any>>({ false, true })),
            te_pool_new<ShaderVariationParam>("DO_DIRECT_LIGHTING", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("USE_SHEEN", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("USE_CLEAR_COAT", std::forward<Vector<std::any>>({false, true})),
            te_pool_new<ShaderVariationParam>("USE_ANISOTROPY", std::forward<Vector<std::any>>({false, true}))
        };

        FillShaderDesc(variationParams, shaderDesc);
        List<ShaderVariation> variations = FillShaderVariations(variationParams);

        if (cull)
            _shaderTransparent = InitShader(variations, shaderDesc, passDesc, "Forward Transparent");
        else
            _shaderTransparentCullNone = InitShader(variations, shaderDesc, passDesc, "Forward Transparent No Culling");
    }

    void BuiltinResources::InitShaderZPrepass()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendTransparentStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderForwardZPrepassDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::FrontToBack;
        shaderDesc.Techniques.push_back(technique);

        _shaderZPrepass = Shader::Create("Z Prepass", shaderDesc);
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

        SHADER_DESC shaderDesc;

        Vector<ShaderVariationParam*> variationParams = {
            te_pool_new<ShaderVariationParam>(std::forward<String>("MSAA_COUNT"), std::forward<Vector<std::any>>({ (UINT32)1, (UINT32)2, (UINT32)4, (UINT32)8 })),
            te_pool_new<ShaderVariationParam>(std::forward<String>("MODE"), std::forward<Vector<std::any>>({ (UINT32)0, (UINT32)1 }))
        };

        FillShaderDesc(variationParams, shaderDesc);
        List<ShaderVariation> variations = FillShaderVariations(variationParams);

        _shaderBlit = InitShader(variations, shaderDesc, passDesc, "Blit");
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

        SHADER_DESC shaderDesc;
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

        SHADER_DESC shaderDesc;
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

        SHADER_DESC shaderDesc;

        Vector<ShaderVariationParam*> variationParams = {
            te_pool_new<ShaderVariationParam>(std::forward<String>("MSAA_COUNT"), std::forward<Vector<std::any>>({ (UINT32)1, (UINT32)2, (UINT32)4, (UINT32)8 })),
            te_pool_new<ShaderVariationParam>(std::forward<String>("GAMMA_ONLY"), std::forward<Vector<std::any>>({ false, true }))
        };

        FillShaderDesc(variationParams, shaderDesc);
        List<ShaderVariation> variations = FillShaderVariations(variationParams);

        _shaderToneMapping = InitShader(variations, shaderDesc, passDesc, "Blit");
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

        SHADER_DESC shaderDesc;

        Vector<ShaderVariationParam*> variationParams = {
            te_pool_new<ShaderVariationParam>(std::forward<String>("MSAA_COUNT"), std::forward<Vector<std::any>>({ (UINT32)1, (UINT32)2, (UINT32)4, (UINT32)8 }))
        };

        FillShaderDesc(variationParams, shaderDesc);
        List<ShaderVariation> variations = FillShaderVariations(variationParams);

        _shaderBloom = InitShader(variations, shaderDesc, passDesc, "Bloom");
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

        SHADER_DESC shaderDesc;
        shaderDesc.Techniques.push_back(technique);

        _shaderMotionBlur = Shader::Create("Motion Blur", shaderDesc);
    }

    void BuiltinResources::InitShaderGaussianBlur()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendTransparentStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderGaussianBlurDesc;
        passDesc.PixelProgramDesc = _pixelShaderGaussianBlurDesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CULL_NONE;

        SHADER_DESC shaderDesc;

        Vector<ShaderVariationParam*> variationParams = {
            te_pool_new<ShaderVariationParam>(std::forward<String>("MSAA_COUNT"), std::forward<Vector<std::any>>({ (UINT32)1, (UINT32)2, (UINT32)4, (UINT32)8 }))
        };

        FillShaderDesc(variationParams, shaderDesc);
        List<ShaderVariation> variations = FillShaderVariations(variationParams);

        _shaderGaussianBlur = InitShader(variations, shaderDesc, passDesc, "Gaussian Blur");
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

        SHADER_DESC shaderDesc;
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

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;
        passDesc.RasterizerStateDesc.polygonMode = PolygonMode::PM_SOLID;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderSelection = Shader::Create("Selection", shaderDesc);
    }

    void BuiltinResources::InitShaderBlitSelection()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderBlitSelectDesc;
        passDesc.PixelProgramDesc = _pixelShaderBlitSelectDesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderBlitSelection = Shader::Create("Selection", shaderDesc);
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

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderHudPicking = Shader::Create("Hud Picking", shaderDesc);
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

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderHudSelection = Shader::Create("Hud Selection", shaderDesc);
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

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderBulletDebug = Shader::Create("Bullet Debug", shaderDesc);
    }

    void BuiltinResources::InitShaderSSAO()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderSSAODesc;
        passDesc.PixelProgramDesc = _pixelShaderSSAODesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
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

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderSSAOBlur = Shader::Create("SSAO Blur", shaderDesc);
    }

    void BuiltinResources::InitShaderSSAODownSample()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderSSAODownSampleDesc;
        passDesc.PixelProgramDesc = _pixelShaderSSAODownSampleDesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderSSAODownSample = Shader::Create("SSAO Down Sample", shaderDesc);
    }

    void BuiltinResources::InitShaderTextureDownsample()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderTextureDownsampleDesc;
        passDesc.PixelProgramDesc = _pixelShaderTextureDownsampleDesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderTextureDownsample = Shader::Create("Texture 2D Down Sample", shaderDesc);
    }

    void BuiltinResources::InitShaderTextureCubeDownsample()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderTextureCubeDownsampleDesc;
        passDesc.PixelProgramDesc = _pixelShaderTextureCubeDownsampleDesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderTextureCubeDownsample = Shader::Create("Texture Cube Down Sample", shaderDesc);
    }

    void BuiltinResources::InitShaderReflectionCubeImportanceSample()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderReflectionCubeImportanceSampleDesc;
        passDesc.PixelProgramDesc = _pixelShaderReflectionCubeImportanceSampleDesc;

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
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

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
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

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
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

        passDesc.DepthStencilStateDesc.StencilEnable = false;
        passDesc.DepthStencilStateDesc.DepthReadEnable = false;
        passDesc.DepthStencilStateDesc.DepthWriteEnable = false;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
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

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        _shaderDecal = Shader::Create("Decal", shaderDesc);
    }

    void BuiltinResources::InitShaderShadowDepthNormal()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderShadowDepthNormalDesc;
        passDesc.PixelProgramDesc = _pixelShaderShadowDepthNormalDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_COUNTERCLOCKWISE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        Vector<ShaderVariationParam*> variationParams = {
            te_pool_new<ShaderVariationParam>(std::forward<String>("SKINNED"), std::forward<Vector<std::any>>({ false, true }))
        };

        FillShaderDesc(variationParams, shaderDesc);
        List<ShaderVariation> variations = FillShaderVariations(variationParams);

        _shaderShadowDepthNormal = InitShader(variations, shaderDesc, passDesc, "Shadow Depth Normal");
    }

    void BuiltinResources::InitShaderShadowDepthCube()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderShadowDepthCubeDesc;
        passDesc.PixelProgramDesc = _pixelShaderShadowDepthCubeDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_COUNTERCLOCKWISE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        Vector<ShaderVariationParam*> variationParams = {
            te_pool_new<ShaderVariationParam>(std::forward<String>("SKINNED"), std::forward<Vector<std::any>>({ false, true }))
        };

        FillShaderDesc(variationParams, shaderDesc);
        List<ShaderVariation> variations = FillShaderVariations(variationParams);

        _shaderShadowDepthCube = InitShader(variations, shaderDesc, passDesc, "Shadow Depth Cube");
    }

    void BuiltinResources::InitShaderShadowDepthDirectional()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderShadowDepthDirectionalDesc;
        passDesc.PixelProgramDesc = _pixelShaderShadowDepthDirectionalDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_COUNTERCLOCKWISE;

        SPtr<Pass> pass = Pass::Create(passDesc);
        SPtr<Technique> technique = Technique::Create("hlsl", { pass });
        technique->Compile();

        SHADER_DESC shaderDesc;
        shaderDesc.QueueType = QueueSortType::BackToFront;
        shaderDesc.Techniques.push_back(technique);

        Vector<ShaderVariationParam*> variationParams = {
            te_pool_new<ShaderVariationParam>(std::forward<String>("SKINNED"), std::forward<Vector<std::any>>({ false, true }))
        };

        FillShaderDesc(variationParams, shaderDesc);
        List<ShaderVariation> variations = FillShaderVariations(variationParams);

        _shaderShadowDepthDirectional = InitShader(variations, shaderDesc, passDesc, "Shadow Depth Directional");
    }

    void BuiltinResources::InitDefaultMaterial()
    {
        MaterialProperties properties;
        _defaultMaterial = Material::Create(GetBuiltinShader(BuiltinShader::Opaque));
        _defaultMaterial->SetProperties(properties);
        _defaultMaterial->SetName("Default Material");
    }

    void BuiltinResources::InitFrameworkIcon()
    {
        auto options = te_shared_ptr_new<TextureImportOptions>();
        HTexture iconTex = gResourceManager().Load<Texture>(ICONS_FOLDER + String("frameworkIcon.png"), options);

        _frameworkIcon = iconTex->GetProperties().AllocBuffer(0, 0);
        iconTex->ReadData(*_frameworkIcon.get());
    }

    BuiltinResources& gBuiltinResources()
    {
        return BuiltinResources::Instance();
    }
}

#include "Resources/TeBuiltinResources.h"
#include "Material/TePass.h"
#include "Material/TeMaterial.h"
#include "Material/TeTechnique.h"
#include "Utility/TeFileStream.h"

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
        InitShaderOpaque();
        InitShaderTransparent();
        InitShaderBlit();
        InitShaderSkybox();
        InitShaderFXAA();
        InitDefaultMaterial();
#endif
    }

    void BuiltinResources::OnShutDown()
    { }

    HShader BuiltinResources::GetBuiltinShader(BuiltinShader type)
    {
        switch(type)
        {
        case BuiltinShader::Opaque:
            return _shaderOpaque;
        case BuiltinShader::Transparent:
            return _shaderTransparent;
        case BuiltinShader::Blit:
            return _shaderBlit;
        case BuiltinShader::Skybox:
            return _shaderSkybox;
        case BuiltinShader::FXAA:
            return _shaderFXAA;
        default:
            break;
        }

        return HShader();
    }

    SPtr<SamplerState> BuiltinResources::GetBuiltinSampler(BuiltinSampler type)
    {
        switch(type)
        {
        case BuiltinSampler::Anisotropic:
            return _anisotropicSamplerState;
        case BuiltinSampler::Bilinear:
            return _bilinearSamplerState; 
        default:
            break;
        }

        return nullptr;
    }

    void BuiltinResources::InitGpuPrograms()
    {
        {
            FileStream shaderFile(SHADERS_FOLDER + String("Raw/Test/Forward_VS.hlsl"));
            _vertexShaderForwardDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderForwardDesc.EntryPoint = "main";
            _vertexShaderForwardDesc.Language = "hlsl";
            _vertexShaderForwardDesc.IncludePath = SHADERS_FOLDER + String("Raw/Test/");
            _vertexShaderForwardDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("Raw/Test/Forward_PS.hlsl"));
            _pixelShaderForwardDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderForwardDesc.EntryPoint = "main";
            _pixelShaderForwardDesc.Language = "hlsl";
            _pixelShaderForwardDesc.IncludePath = SHADERS_FOLDER + String("Raw/Test/");
            _pixelShaderForwardDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("Raw/Test/Blit_VS.hlsl"));
            _vertexShaderBlitDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderBlitDesc.EntryPoint = "main";
            _vertexShaderBlitDesc.Language = "hlsl";
            _vertexShaderBlitDesc.IncludePath = "";
            _vertexShaderBlitDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("Raw/Test/Blit_PS.hlsl"));
            _pixelShaderBlitDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderBlitDesc.EntryPoint = "main";
            _pixelShaderBlitDesc.Language = "hlsl";
            _pixelShaderBlitDesc.IncludePath = "";
            _pixelShaderBlitDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("Raw/Test/Skybox_VS.hlsl"));
            _vertexShaderSkyboxDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderSkyboxDesc.EntryPoint = "main";
            _vertexShaderSkyboxDesc.Language = "hlsl";
            _vertexShaderSkyboxDesc.IncludePath = "";
            _vertexShaderSkyboxDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("Raw/Test/Skybox_PS.hlsl"));
            _pixelShaderSkyboxDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderSkyboxDesc.EntryPoint = "main";
            _pixelShaderSkyboxDesc.Language = "hlsl";
            _pixelShaderSkyboxDesc.IncludePath = "";
            _pixelShaderSkyboxDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("Raw/Test/FXAA_VS.hlsl"));
            _vertexShaderFXAADesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderFXAADesc.EntryPoint = "main";
            _vertexShaderFXAADesc.Language = "hlsl";
            _vertexShaderFXAADesc.IncludePath = SHADERS_FOLDER + String("Raw/Test/");
            _vertexShaderFXAADesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile(SHADERS_FOLDER + String("Raw/Test/FXAA_PS.hlsl"));
            _pixelShaderFXAADesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderFXAADesc.EntryPoint = "main";
            _pixelShaderFXAADesc.Language = "hlsl";
            _pixelShaderFXAADesc.IncludePath = SHADERS_FOLDER + String("Raw/Test/");
            _pixelShaderFXAADesc.Source = shaderFile.GetAsString();
        }
    }
    void BuiltinResources::InitStates()
    {
        _blendTransparentStateDesc.AlphaToCoverageEnable = true;
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
        _anisotropicSamplerStateDesc.MaxAnisotropy = 4;

        _bilinearSamplerStateDesc.AddressMode = UVWAddressingMode();
        _bilinearSamplerStateDesc.MinFilter = FO_POINT;
        _bilinearSamplerStateDesc.MagFilter = FO_POINT;
        _bilinearSamplerStateDesc.MipFilter = FO_POINT;
    }

    void BuiltinResources::InitShaderDesc()
    {
        {
            SHADER_DATA_PARAM_DESC gViewDirDesc("gViewDir", "gViewDir", GPDT_FLOAT3);
            SHADER_DATA_PARAM_DESC gViewOriginDesc("gViewOrigin", "gViewOrigin", GPDT_FLOAT3);
            SHADER_DATA_PARAM_DESC gMatViewProjDesc("gMatViewProj", "gMatViewProj", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatViewDesc("gMatView", "gMatView", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatProjDesc("gMatProj", "gMatProj", GPDT_MATRIX_4X4);

            SHADER_DATA_PARAM_DESC gMatWorldDesc("gMatWorld", "gMatWorld", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatInvWorldDesc("gMatInvWorld", "gMatInvWorld", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatWorldNoScaleDesc("gMatWorldNoScale", "gMatWorldNoScale", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatInvWorldNoScaleDesc("gMatInvWorldNoScale", "gMatInvWorldNoScale", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gMatPrevWorldDesc("gMatPrevWorld", "gMatPrevWorld", GPDT_MATRIX_4X4);
            SHADER_DATA_PARAM_DESC gLayerDesc("gLayer", "gLayer", GPDT_INT1);

            SHADER_DATA_PARAM_DESC gTime("gTime", "gTime", GPDT_FLOAT1);

            SHADER_DATA_PARAM_DESC gMatWorldViewProj("gMatWorldViewProj", "gMatWorldViewProj", GPDT_MATRIX_4X4);

            SHADER_DATA_PARAM_DESC gInstanceData("gInstanceData", "gInstanceData", GPDT_STRUCT);
            gInstanceData.ElementSize = sizeof(PerInstanceData);

            SHADER_DATA_PARAM_DESC gAmbient("gAmbient", "gAmbient", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gDiffuse("gDiffuse", "gDiffuse", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gSpecular("gSpecular", "gSpecular", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gEmissive("gEmissive", "gEmissive", GPDT_FLOAT4);
            SHADER_DATA_PARAM_DESC gUseDiffuseMap("gUseDiffuseMap", "gUseDiffuseMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseEmissiveMap("gUseEmissiveMap", "gUseEmissiveMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseNormalMap("gUseNormalMap", "gUseNormalMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseSpecularMap("gUseSpecularMap", "gUseSpecularMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseBumpMap("gUseBumpMap", "gUseBumpMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseParallaxMap("gUseParallaxMap", "gUseParallaxMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseTransparencyMap("gUseTransparencyMap", "gUseTransparencyMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseReflectionMap("gUseReflectionMap", "gUseReflectionMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gUseOcclusionMap("gUseOcclusionMap", "gUseOcclusionMap", GPDT_INT1);
            SHADER_DATA_PARAM_DESC gSpecularPower("gSpecularPower", "gSpecularPower", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gTransparency("gTransparency", "gTransparency", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gIndexOfRefraction("gIndexOfRefraction", "gIndexOfRefraction", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gReflection("gReflection", "gReflection", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gAbsorbance("gAbsorbance", "gAbsorbance", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gBumpScale("gBumScale", "gBumScale", GPDT_FLOAT1);
            SHADER_DATA_PARAM_DESC gAlphaThreshold("gAlphaThreshold", "gAlphaThreshold", GPDT_FLOAT1);

            SHADER_OBJECT_PARAM_DESC anisotropicSamplerDesc("AnisotropicSampler", "AnisotropicSampler", GPOT_SAMPLER2D);
            SHADER_OBJECT_PARAM_DESC diffuseMapDesc("DiffuseMap", "DiffuseMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC emissiveMapDesc("EmissiveMap", "EmissiveMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC normalMapDesc("NormalMap", "NormalMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC specularMapDesc("SpecularMap", "SpecularMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC bumpMapDesc("BumpMap", "BumpMap", GPOT_TEXTURE2D);
            SHADER_OBJECT_PARAM_DESC transparencyMapDesc("TransparencyMap", "TransparencyMap", GPOT_TEXTURE2D);

            SHADER_DATA_PARAM_DESC gLightsDesc("gLights", "gLights", GPDT_STRUCT);
            gLightsDesc.ElementSize = sizeof(LightData);
            SHADER_DATA_PARAM_DESC gLightsNumberDesc("gLightsNumber", "gLightsNumber", GPDT_INT1);

            _forwardShaderDesc.AddParameter(gViewDirDesc);
            _forwardShaderDesc.AddParameter(gViewOriginDesc);
            _forwardShaderDesc.AddParameter(gMatViewProjDesc);
            _forwardShaderDesc.AddParameter(gMatViewDesc);
            _forwardShaderDesc.AddParameter(gMatProjDesc);

            _forwardShaderDesc.AddParameter(gInstanceData);

            _forwardShaderDesc.AddParameter(gMatWorldDesc);
            _forwardShaderDesc.AddParameter(gMatInvWorldDesc);
            _forwardShaderDesc.AddParameter(gMatWorldNoScaleDesc);
            _forwardShaderDesc.AddParameter(gMatInvWorldNoScaleDesc);
            _forwardShaderDesc.AddParameter(gMatPrevWorldDesc);
            _forwardShaderDesc.AddParameter(gLayerDesc);
            
            _forwardShaderDesc.AddParameter(gAmbient);
            _forwardShaderDesc.AddParameter(gDiffuse);
            _forwardShaderDesc.AddParameter(gEmissive);
            _forwardShaderDesc.AddParameter(gSpecular);
            _forwardShaderDesc.AddParameter(gUseDiffuseMap);
            _forwardShaderDesc.AddParameter(gUseEmissiveMap);
            _forwardShaderDesc.AddParameter(gUseNormalMap);
            _forwardShaderDesc.AddParameter(gUseSpecularMap);
            _forwardShaderDesc.AddParameter(gUseBumpMap);
            _forwardShaderDesc.AddParameter(gUseParallaxMap);
            _forwardShaderDesc.AddParameter(gUseTransparencyMap);
            _forwardShaderDesc.AddParameter(gUseReflectionMap);
            _forwardShaderDesc.AddParameter(gUseOcclusionMap);
            _forwardShaderDesc.AddParameter(gSpecularPower);
            _forwardShaderDesc.AddParameter(gTransparency);
            _forwardShaderDesc.AddParameter(gIndexOfRefraction);
            _forwardShaderDesc.AddParameter(gReflection);
            _forwardShaderDesc.AddParameter(gAbsorbance);
            _forwardShaderDesc.AddParameter(gBumpScale);
            _forwardShaderDesc.AddParameter(gAlphaThreshold);

            _forwardShaderDesc.AddParameter(gTime);

            _forwardShaderDesc.AddParameter(gMatWorldViewProj);

            _forwardShaderDesc.AddParameter(anisotropicSamplerDesc);
            _forwardShaderDesc.AddParameter(diffuseMapDesc);
            _forwardShaderDesc.AddParameter(emissiveMapDesc);
            _forwardShaderDesc.AddParameter(normalMapDesc);
            _forwardShaderDesc.AddParameter(specularMapDesc);
            _forwardShaderDesc.AddParameter(bumpMapDesc);
            _forwardShaderDesc.AddParameter(transparencyMapDesc);

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

            SHADER_OBJECT_PARAM_DESC anisotropicSamplerDesc("AnisotropicSampler", "AnisotropicSampler", GPOT_SAMPLER2D);

            SHADER_OBJECT_PARAM_DESC textureMapDesc("TextureMap", "TextureMap", GPOT_TEXTURE2D);

            _skyboxShaderDesc.AddParameter(gClearColor);
            _skyboxShaderDesc.AddParameter(gUseTexture);
            
            _skyboxShaderDesc.AddParameter(anisotropicSamplerDesc);

            _skyboxShaderDesc.AddParameter(textureMapDesc);
        }

        {
            SHADER_DATA_PARAM_DESC gInvTexSize("gInvTexSize", "gInvTexSize", GPDT_FLOAT2);
            SHADER_OBJECT_PARAM_DESC sourceMapDesc("SourceMap", "SourceMap", GPOT_TEXTURE2D);

            _FXAAShaderDesc.AddParameter(gInvTexSize);
            _FXAAShaderDesc.AddParameter(sourceMapDesc);
        }
    }

    void BuiltinResources::InitSamplers()
    {
        _anisotropicSamplerState = SamplerState::Create(_anisotropicSamplerStateDesc);
        _bilinearSamplerState = SamplerState::Create(_bilinearSamplerStateDesc);
    }

    void BuiltinResources::InitShaderOpaque()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderForwardDesc;
        passDesc.PixelProgramDesc = _pixelShaderForwardDesc;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _forwardShaderDesc;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderOpaque = Shader::Create("Forward_Opaque", shaderDesc);
    }

    void BuiltinResources::InitShaderTransparent()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendTransparentStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderForwardDesc;
        passDesc.PixelProgramDesc = _pixelShaderForwardDesc;

        passDesc.RasterizerStateDesc.cullMode = CullingMode::CULL_NONE;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _forwardShaderDesc;
        shaderDesc.Flags = (UINT32)ShaderFlag::Transparent;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());
        
        _shaderTransparent = Shader::Create("Forward_Transparent", shaderDesc);
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

    void BuiltinResources::InitDefaultMaterial()
    {
        MaterialProperties properties;
        _defaultMaterial = Material::Create(_shaderOpaque);
        _defaultMaterial->SetProperties(properties);
    }

    BuiltinResources& gBuiltinResources()
    {
        return BuiltinResources::Instance();
    }
}

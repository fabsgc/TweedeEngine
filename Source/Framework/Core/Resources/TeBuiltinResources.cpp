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
    { }

    void BuiltinResources::OnStartUp()
    {
        InitGpuPrograms();
        InitStates();
        InitShaderDesc();
        InitShaderOpaque();
        InitShaderTransparent();
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
        default:
            break;
        }

        return HShader();
    }

    void BuiltinResources::InitGpuPrograms()
    {
        {
            FileStream shaderFile("Data/Shaders/Raw/Test/Texture_VS.hlsl");
            _vertexShaderProgramDesc.Type = GPT_VERTEX_PROGRAM;
            _vertexShaderProgramDesc.EntryPoint = "main";
            _vertexShaderProgramDesc.Language = "hlsl";
            _vertexShaderProgramDesc.IncludePath = "Data/Shaders/Raw/Test/";
            _vertexShaderProgramDesc.Source = shaderFile.GetAsString();
        }

        {
            FileStream shaderFile("Data/Shaders/Raw/Test/Texture_PS.hlsl");
            _pixelShaderProgramDesc.Type = GPT_PIXEL_PROGRAM;
            _pixelShaderProgramDesc.EntryPoint = "main";
            _pixelShaderProgramDesc.Language = "hlsl";
            _pixelShaderProgramDesc.IncludePath = "Data/Shaders/Raw/Test/";
            _pixelShaderProgramDesc.Source = shaderFile.GetAsString();
        }
    }
    void BuiltinResources::InitStates()
    {
        _blendTransparentStateDesc;
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

        _samplerStateDesc.AddressMode = UVWAddressingMode();
        _samplerStateDesc.MinFilter = FO_ANISOTROPIC;
        _samplerStateDesc.MagFilter = FO_ANISOTROPIC;
        _samplerStateDesc.MipFilter = FO_ANISOTROPIC;
        _samplerStateDesc.MaxAnisotropy = 16;
    }

    void BuiltinResources::InitShaderDesc()
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
        SHADER_DATA_PARAM_DESC gUseTransparencyMap("gUseTransparencyMap", "gUseTransparencyMap", GPDT_INT1);
        SHADER_DATA_PARAM_DESC gSpecularPower("gSpecularPower", "gSpecularPower", GPDT_FLOAT1);
        SHADER_DATA_PARAM_DESC gTransparency("gTransparency", "gTransparency", GPDT_FLOAT1);
        SHADER_DATA_PARAM_DESC gIndexOfRefraction("gIndexOfRefraction", "gIndexOfRefraction", GPDT_FLOAT1);
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

        _shaderDesc.AddParameter(gViewDirDesc);
        _shaderDesc.AddParameter(gViewOriginDesc);
        _shaderDesc.AddParameter(gMatViewProjDesc);
        _shaderDesc.AddParameter(gMatViewDesc);
        _shaderDesc.AddParameter(gMatProjDesc);

        _shaderDesc.AddParameter(gInstanceData);

        _shaderDesc.AddParameter(gMatWorldDesc);
        _shaderDesc.AddParameter(gMatInvWorldDesc);
        _shaderDesc.AddParameter(gMatWorldNoScaleDesc);
        _shaderDesc.AddParameter(gMatInvWorldNoScaleDesc);
        _shaderDesc.AddParameter(gMatPrevWorldDesc);
        _shaderDesc.AddParameter(gLayerDesc);
        
        _shaderDesc.AddParameter(gAmbient);
        _shaderDesc.AddParameter(gDiffuse);
        _shaderDesc.AddParameter(gEmissive);
        _shaderDesc.AddParameter(gSpecular);
        _shaderDesc.AddParameter(gUseDiffuseMap);
        _shaderDesc.AddParameter(gUseEmissiveMap);
        _shaderDesc.AddParameter(gUseNormalMap);
        _shaderDesc.AddParameter(gUseSpecularMap);
        _shaderDesc.AddParameter(gUseBumpMap);
        _shaderDesc.AddParameter(gUseTransparencyMap);
        _shaderDesc.AddParameter(gSpecularPower);
        _shaderDesc.AddParameter(gTransparency);
        _shaderDesc.AddParameter(gIndexOfRefraction);
        _shaderDesc.AddParameter(gAbsorbance);
        _shaderDesc.AddParameter(gBumpScale);
        _shaderDesc.AddParameter(gAlphaThreshold);

        _shaderDesc.AddParameter(gTime);

        _shaderDesc.AddParameter(gMatWorldViewProj);

        _shaderDesc.AddParameter(anisotropicSamplerDesc);
        _shaderDesc.AddParameter(diffuseMapDesc);
        _shaderDesc.AddParameter(emissiveMapDesc);
        _shaderDesc.AddParameter(normalMapDesc);
        _shaderDesc.AddParameter(specularMapDesc);
        _shaderDesc.AddParameter(bumpMapDesc);
        _shaderDesc.AddParameter(transparencyMapDesc);
    }

    void BuiltinResources::InitAnisotropicSampler()
    {
        _anisotropicSamplerState = SamplerState::Create(_samplerStateDesc);
    }

    void BuiltinResources::InitShaderOpaque()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendOpaqueStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderProgramDesc;
        passDesc.PixelProgramDesc = _pixelShaderProgramDesc;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _shaderDesc;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());

        _shaderOpaque = Shader::Create("Opaque", shaderDesc);
    }

    void BuiltinResources::InitShaderTransparent()
    {
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = _blendTransparentStateDesc;
        passDesc.DepthStencilStateDesc = _depthStencilStateDesc;
        passDesc.RasterizerStateDesc = _rasterizerStateDesc;
        passDesc.VertexProgramDesc = _vertexShaderProgramDesc;
        passDesc.PixelProgramDesc = _pixelShaderProgramDesc;

        HPass pass = Pass::Create(passDesc);
        HTechnique technique = Technique::Create("hlsl", { pass.GetInternalPtr() });
        technique->Compile();

        SHADER_DESC shaderDesc = _shaderDesc;
        shaderDesc.Flags = (UINT32)ShaderFlag::Transparent;
        shaderDesc.Techniques.push_back(technique.GetInternalPtr());
        
        _shaderTransparent = Shader::Create("Transparent", shaderDesc);
    }

    BuiltinResources& gBuiltinResources()
    {
        return BuiltinResources::Instance();
    }
}

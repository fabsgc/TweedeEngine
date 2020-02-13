#include "TeApplication.h"

#include "Error/TeConsole.h"
#include "Utility/TeFileStream.h"

#include "Resources/TeResourceManager.h"

#include "Input/TeInput.h"
#include "Input/TeVirtualInput.h"

#include "Importer/TeImporter.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"
#include "Importer/TeShaderImportOptions.h"

#include "Renderer/TeRenderer.h"
#include "Renderer/TeCamera.h"

#include "Scene/TeSceneManager.h"

#include "Mesh/TeMesh.h"

#include "Scene/TeSceneObject.h"
#include "Components/TeCCamera.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCLight.h"
#include "Components/TeCSkybox.h"
#include "Components/TeCCameraFlyer.h"

#include "Material/TeMaterial.h"
#include "Material/TeShader.h"
#include "Material/TeTechnique.h"
#include "Material/TePass.h"

#include "Math/TeVector4.h"

namespace te
{
    struct PerInstanceData
    {
        Matrix4 gMatWorld;
        Matrix4 gMatInvWorld;
        Matrix4 gMatWorldNoScale;
        Matrix4 gMatInvWorldNoScale;
        Matrix4 gMatPrevWorld;
        UINT32  gLayer;
    };

    TE_MODULE_STATIC_MEMBER(Application)

    void Application::InitInputHandling()
    {
        auto inputConfig = gVirtualInput().GetConfiguration();

        inputConfig->RegisterButton("Forward", TE_W);
        inputConfig->RegisterButton("Back", TE_S);
        inputConfig->RegisterButton("Left", TE_A);
        inputConfig->RegisterButton("Right", TE_D);
        inputConfig->RegisterButton("Forward", TE_UP);
        inputConfig->RegisterButton("Back", TE_DOWN);
        inputConfig->RegisterButton("Left", TE_LEFT);
        inputConfig->RegisterButton("Right", TE_RIGHT);
        inputConfig->RegisterButton("FastMove", TE_LSHIFT);
        inputConfig->RegisterButton("RotateObj", TE_MOUSE_LEFT);
        inputConfig->RegisterButton("RotateCam", TE_MOUSE_RIGHT);

        inputConfig->RegisterAxis("Horizontal", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
        inputConfig->RegisterAxis("Vertical", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));
    }

    void Application::InitShader()
    {
        GPU_PROGRAM_DESC vertexShaderProgramDesc;
        {
            FileStream shaderFile("Data/Shaders/Raw/Test/Texture_VS.hlsl");
            vertexShaderProgramDesc.Type = GPT_VERTEX_PROGRAM;
            vertexShaderProgramDesc.EntryPoint = "main";
            vertexShaderProgramDesc.Language = "hlsl";
            vertexShaderProgramDesc.IncludePath = "Data/Shaders/Raw/Test/";
            vertexShaderProgramDesc.Source = shaderFile.GetAsString();
        }

        GPU_PROGRAM_DESC pixelShaderProgramDesc;
        {
            FileStream shaderFile("Data/Shaders/Raw/Test/Texture_PS.hlsl");
            pixelShaderProgramDesc.Type = GPT_PIXEL_PROGRAM;
            pixelShaderProgramDesc.EntryPoint = "main";
            pixelShaderProgramDesc.Language = "hlsl";
            pixelShaderProgramDesc.IncludePath = "Data/Shaders/Raw/Test/";
            pixelShaderProgramDesc.Source = shaderFile.GetAsString();
        }

        BLEND_STATE_DESC blendDesc;

        RASTERIZER_STATE_DESC rastDesc;
        rastDesc.polygonMode = PM_SOLID;
        rastDesc.cullMode = CULL_CLOCKWISE;
        rastDesc.multisampleEnable = true;
        rastDesc.depthClipEnable = true;

        DEPTH_STENCIL_STATE_DESC depthDesc;
        depthDesc.DepthReadEnable = true;
        depthDesc.DepthWriteEnable = true;
        depthDesc.StencilEnable = true;

        depthDesc.FrontStencilFailOp = SOP_KEEP;
        depthDesc.FrontStencilZFailOp = SOP_INCREMENT_WRAP;
        depthDesc.FrontStencilPassOp = SOP_KEEP;
        depthDesc.FrontStencilComparisonFunc = CMPF_ALWAYS_PASS;

        depthDesc.BackStencilFailOp = SOP_KEEP;
        depthDesc.BackStencilZFailOp = SOP_DECREMENT_WRAP;
        depthDesc.BackStencilPassOp = SOP_KEEP;
        depthDesc.BackStencilComparisonFunc = CMPF_ALWAYS_PASS;

        SAMPLER_STATE_DESC samplerDesc;
        samplerDesc.AddressMode = UVWAddressingMode();
        samplerDesc.MinFilter = FO_ANISOTROPIC;
        samplerDesc.MagFilter = FO_ANISOTROPIC;
        samplerDesc.MipFilter = FO_ANISOTROPIC;
        samplerDesc.MaxAnisotropy = 16;

        SPtr<BlendState> _blendState = BlendState::Create(blendDesc);
        SPtr<RasterizerState> _rasterizerState = RasterizerState::Create(rastDesc);
        SPtr<DepthStencilState> _depthStencilState = DepthStencilState::Create(depthDesc);
        SPtr<SamplerState> _samplerState = SamplerState::Create(samplerDesc);

        PIPELINE_STATE_DESC pipeDesc;
        pipeDesc.blendState = _blendState;
        pipeDesc.rasterizerState = _rasterizerState;
        pipeDesc.depthStencilState = _depthStencilState;
        // ######################################################

        // ######################################################
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = blendDesc;
        passDesc.DepthStencilStateDesc = depthDesc;
        passDesc.RasterizerStateDesc = rastDesc;
        passDesc.VertexProgramDesc = vertexShaderProgramDesc;
        passDesc.PixelProgramDesc = pixelShaderProgramDesc;

        _pass = Pass::Create(passDesc);
        _technique = Technique::Create("hlsl", { _pass.GetInternalPtr() });
        _technique->Compile();

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
        SHADER_DATA_PARAM_DESC gUseSpecularMap("gUseSpecularMap", "gUseSpecularMap", GPDT_INT1);
        SHADER_DATA_PARAM_DESC gUseNormalMap("gUseNormalMap", "gUseNormalMap", GPDT_INT1);
        SHADER_DATA_PARAM_DESC gSpecularPower("gSpecularPower", "gSpecularPower", GPDT_FLOAT1);
        SHADER_DATA_PARAM_DESC gTransparency("gTransparency", "gTransparency", GPDT_FLOAT1);
        SHADER_DATA_PARAM_DESC gAbsorbance("gAbsorbance", "gAbsorbance", GPDT_FLOAT1);

        SHADER_OBJECT_PARAM_DESC anisotropicSamplerDesc("AnisotropicSampler", "AnisotropicSampler", GPOT_SAMPLER2D);
        SHADER_OBJECT_PARAM_DESC colorTextureDesc("ColorTexture", "ColorTexture", GPOT_TEXTURE2D);

        SHADER_DESC shaderDesc;
        shaderDesc.AddParameter(gViewDirDesc);
        shaderDesc.AddParameter(gViewOriginDesc);
        shaderDesc.AddParameter(gMatViewProjDesc);
        shaderDesc.AddParameter(gMatViewDesc);
        shaderDesc.AddParameter(gMatProjDesc);

        shaderDesc.AddParameter(gInstanceData);

        shaderDesc.AddParameter(gMatWorldDesc);
        shaderDesc.AddParameter(gMatInvWorldDesc);
        shaderDesc.AddParameter(gMatWorldNoScaleDesc);
        shaderDesc.AddParameter(gMatInvWorldNoScaleDesc);
        shaderDesc.AddParameter(gMatPrevWorldDesc);
        shaderDesc.AddParameter(gLayerDesc);
        
        shaderDesc.AddParameter(gAmbient);
        shaderDesc.AddParameter(gDiffuse);
        shaderDesc.AddParameter(gSpecular);
        shaderDesc.AddParameter(gEmissive);
        shaderDesc.AddParameter(gUseDiffuseMap);
        shaderDesc.AddParameter(gUseSpecularMap);
        shaderDesc.AddParameter(gUseNormalMap);
        shaderDesc.AddParameter(gSpecularPower);
        shaderDesc.AddParameter(gTransparency);
        shaderDesc.AddParameter(gAbsorbance);

        shaderDesc.AddParameter(gTime);

        shaderDesc.AddParameter(gMatWorldViewProj);

        shaderDesc.AddParameter(anisotropicSamplerDesc);
        shaderDesc.AddParameter(colorTextureDesc);

        shaderDesc.Techniques.push_back(_technique.GetInternalPtr());

        _shader = Shader::Create("Texture", shaderDesc);
        _shader->SetName("Shader");
    }

    void Application::InitMaterials()
    {
        _materials =
        {
            {
                "arch",
                "Data/Textures/Sponza/arch/arch_COLOR.jpeg",
                "Data/Textures/Sponza/arch/arch_NRM.jpeg",
                "Data/Textures/Sponza/arch/arch_SPEC.jpeg"
            },
            {
                "background",
                "Data/Textures/Sponza/background/background_COLOR.jpeg",
                "Data/Textures/Sponza/background/background_NRM.jpeg",
                "Data/Textures/Sponza/background/background_SPEC.jpeg"
            },
            {
                "bricks",
                "Data/Textures/Sponza/bricks/bricks_COLOR.jpeg",
                "Data/Textures/Sponza/bricks/bricks_NRM.jpeg",
                "Data/Textures/Sponza/bricks/bricks_SPEC.jpeg"
            },
            {
                "ceiling",
                "Data/Textures/Sponza/ceiling/ceiling_COLOR.jpeg",
                "Data/Textures/Sponza/ceiling/ceiling_NRM.jpeg",
                "Data/Textures/Sponza/ceiling/ceiling_SPEC.jpeg"
            },
            {
                "chain",
                "Data/Textures/Sponza/chain/chain_COLOR.jpeg",
                "Data/Textures/Sponza/chain/chain_NRM.jpeg",
                "Data/Textures/Sponza/chain/chain_SPEC.jpeg"
            },
            {
                "column_a",
                "Data/Textures/Sponza/column_a/column_a_COLOR.jpeg",
                "Data/Textures/Sponza/column_a/column_a_NRM.jpeg",
                "Data/Textures/Sponza/column_a/column_a_SPEC.jpeg"
            },
            {
                "column_b",
                "Data/Textures/Sponza/column_b/column_b_COLOR.jpeg",
                "Data/Textures/Sponza/column_b/column_b_NRM.jpeg",
                "Data/Textures/Sponza/column_b/column_b_SPEC.jpeg"
            },
            {
                "column_c",
                "Data/Textures/Sponza/column_c/column_c_COLOR.jpeg",
                "Data/Textures/Sponza/column_c/column_c_NRM.jpeg",
                "Data/Textures/Sponza/column_c/column_c_SPEC.jpeg"
            },
            {
                "details",
                "Data/Textures/Sponza/details/details_COLOR.jpeg",
                "Data/Textures/Sponza/details/details_NRM.jpeg",
                "Data/Textures/Sponza/details/details_SPEC.jpeg"
            },
            {
                "fabric_a",
                "Data/Textures/Sponza/fabric_a/fabric_a_COLOR.jpeg",
                "Data/Textures/Sponza/fabric_a/fabric_a_NRM.jpeg",
                "Data/Textures/Sponza/fabric_a/fabric_a_SPEC.jpeg"
            },
            {
                "fabric_c",
                "Data/Textures/Sponza/fabric_c/fabric_c_COLOR.jpeg",
                "Data/Textures/Sponza/fabric_c/fabric_c_NRM.jpeg",
                "Data/Textures/Sponza/fabric_c/fabric_c_SPEC.jpeg"
            },
            {
                "fabric_d",
                "Data/Textures/Sponza/fabric_d/fabric_d_COLOR.jpeg",
                "Data/Textures/Sponza/fabric_a/fabric_a_NRM.jpeg",
                "Data/Textures/Sponza/fabric_a/fabric_a_SPEC.jpeg"
            },
            {
                "fabric_e",
                "Data/Textures/Sponza/fabric_e/fabric_e_COLOR.jpeg",
                "Data/Textures/Sponza/fabric_a/fabric_a_NRM.jpeg",
                "Data/Textures/Sponza/fabric_a/fabric_a_SPEC.jpeg"
            },
            {
                "fabric_f",
                "Data/Textures/Sponza/fabric_f/fabric_f_COLOR.jpeg",
                "Data/Textures/Sponza/fabric_c/fabric_c_NRM.jpeg",
                "Data/Textures/Sponza/fabric_c/fabric_c_SPEC.jpeg"
            },
            {
                "fabric_g",
                "Data/Textures/Sponza/fabric_g/fabric_g_COLOR.jpeg",
                "Data/Textures/Sponza/fabric_c/fabric_c_NRM.jpeg",
                "Data/Textures/Sponza/fabric_c/fabric_c_SPEC.jpeg"
            },
            {
                "flagpole",
                "Data/Textures/Sponza/flagpole/flagpole_COLOR.jpeg",
                "Data/Textures/Sponza/flagpole/flagpole_NRM.jpeg",
                "Data/Textures/Sponza/flagpole/flagpole_SPEC.jpeg"
            },
            {
                "floor",
                "Data/Textures/Sponza/floor/floor_COLOR.jpeg",
                "Data/Textures/Sponza/floor/floor_NRM.jpeg",
                "Data/Textures/Sponza/floor/floor_SPEC.jpeg"
            },
            {
                "leaf",
                "Data/Textures/Sponza/leaf/leaf_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/leaf/leaf_SPEC.jpeg"
            },
            {
                "lion",
                "Data/Textures/Sponza/lion/lion_COLOR.jpeg",
                "Data/Textures/Sponza/lion/lion_NRM.jpeg",
                "Data/Textures/Sponza/lion/lion_SPEC.jpeg"
            },
            {
                "roof",
                "Data/Textures/Sponza/roof/roof_COLOR.jpeg",
                "Data/Textures/Sponza/roof/roof_NRM.jpeg",
                "Data/Textures/Sponza/roof/roof_SPEC.jpeg"
            },
            {
                "vase",
                "Data/Textures/Sponza/vase/vase_COLOR.jpeg",
                "Data/Textures/Sponza/vase/vase_NRM.jpeg",
                "Data/Textures/Sponza/vase/vase_SPEC.jpeg"
            },
            {
                "vase_hanging",
                "Data/Textures/Sponza/vase_hanging/vase_hanging_COLOR.jpeg",
                "Data/Textures/Sponza/vase_hanging/vase_hanging_NRM.jpeg",
                "Data/Textures/Sponza/vase_hanging/vase_hanging_SPEC.jpeg"
            },
            {
                "vase_plant",
                "Data/Textures/Sponza/vase_plant/vase_plant_COLOR.jpeg",
                "",
                "Data/Textures/Sponza/vase_plant/vase_plant_SPEC.jpeg"
            },
            {
                "vase_round",
                "Data/Textures/Sponza/vase_round/vase_round_COLOR.jpeg",
                "Data/Textures/Sponza/vase_round/vase_round_NRM.jpeg",
                "Data/Textures/Sponza/vase_round/vase_round_SPEC.jpeg"
            }
        };

        auto textureDiffuseImportOptions = TextureImportOptions::Create();
        textureDiffuseImportOptions->CpuCached = false;
        textureDiffuseImportOptions->GenerateMips = true;
        textureDiffuseImportOptions->MaxMip = 10;

        auto textureImportOptions = TextureImportOptions::Create();
        textureImportOptions->CpuCached = false;
        textureImportOptions->GenerateMips = true;
        textureImportOptions->MaxMip = 10;

        TextureSurface surface;
        surface.MipLevel = 0;
        surface.NumMipLevels = 10;

        for (auto& material : _materials)
        {
            material.MaterialProp.SpecularPower = 16.0f;
            material.MaterialProp.Ambient = Color(1.0f, 1.0f, 1.0f, 0.75f);

            if (material.Diffuse != "")
            {
                material.DiffuseTexture = gResourceManager().Load<Texture>(material.Diffuse, textureDiffuseImportOptions);
                material.MaterialProp.UseDiffuseMap = true;
            }

            if (material.Normal != "")
            {
                material.NormalTexture = gResourceManager().Load<Texture>(material.Normal, textureImportOptions);
                material.MaterialProp.UseNormalMap = true;
            }

            if (material.Specular != "")
            {
                material.SpecularTexture = gResourceManager().Load<Texture>(material.Specular, textureImportOptions);
                material.MaterialProp.UseSpecularMap = true;
            }

            material.MaterialElement = Material::Create(_shader);
            material.MaterialElement->SetName(material.Name);
            material.MaterialElement->SetSamplerState("AnisotropicSampler", _samplerState);

            if (material.Diffuse != "") material.MaterialElement->SetTexture("DiffuseMap", material.DiffuseTexture, surface);
            if (material.Normal != "") material.MaterialElement->SetTexture("NormalMap", material.NormalTexture, surface);
            if (material.Specular != "") material.MaterialElement->SetTexture("SpecularMap", material.SpecularTexture, surface);

            material.MaterialElement->SetProperties(material.MaterialProp);
        }
    }

    void Application::InitMesh()
    {
        auto meshImportOptions = MeshImportOptions::Create();
        meshImportOptions->ImportNormals = true;
        meshImportOptions->ImportTangents = true;
        meshImportOptions->CpuCached = false;

        _sponzaMesh = gResourceManager().Load<Mesh>("Data/Meshes/Sponza/sponza.obj", meshImportOptions);

        TE_PRINT((_sponzaMesh.GetHandleData())->data);
        TE_PRINT((_sponzaMesh.GetHandleData())->uuid.ToString());
    }

    void Application::InitScene()
    {
        _sceneCameraSO = SceneObject::Create("SceneCamera");
        _sceneCameraFlyer = _sceneCameraSO->AddComponent<CCameraFlyer>();
        _sceneCamera = _sceneCameraSO->AddComponent<CCamera>();
        _sceneCamera->GetViewport()->SetClearColorValue(Color(0.17f, 0.64f, 1.0f, 1.0f));
        _sceneCamera->GetViewport()->SetTarget(gCoreApplication().GetWindow());
        _sceneCamera->SetMSAACount(8);
        _sceneCamera->SetProjectionType(ProjectionType::PT_PERSPECTIVE);
        _sceneCamera->SetMain(true);
        _sceneCamera->Initialize();

        _sceneSponzaSO = SceneObject::Create("Sponza");
        _sponzaRenderable = _sceneSponzaSO->AddComponent<CRenderable>();
        _sponzaRenderable->SetMesh(_sponzaMesh);
        
        for (auto& material : _materials)
        {
            _sponzaRenderable->SetMaterial(material.Name, material.MaterialElement);
        }

        _sponzaRenderable->Initialize();

        _sceneCameraSO->SetPosition(Vector3(0.0f, 2.0f, 0.0f));
        _sceneCameraSO->LookAt(Vector3(0.0f, 1.5f, -3.0f));
    }

    void Application::PostStartUp()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        InitInputHandling();
        InitShader();
        InitMaterials();
        InitMesh();
        InitScene();
#endif

        // ######################################################
        gRenderer()->BatchRenderables();
        // ######################################################

        // ######################################################
        gSceneManager().SetMainRenderTarget(gCoreApplication().GetWindow());
        // ######################################################
    }

    void Application::PreShutDown()
    {
        _blendState = nullptr;
        _rasterizerState = nullptr;
        _depthStencilState = nullptr;
        _samplerState = nullptr;
    }

    void Application::PreUpdate()
    { }

    void Application::PostUpdate()
    { }
}

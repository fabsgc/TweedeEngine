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

#include "Utility/TeTime.h"

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

    void Application::PostStartUp()
    {
        // ######################################################
        auto inputConfig = gVirtualInput().GetConfiguration();

        // Camera controls for buttons (digital 0-1 input, e.g. keyboard or gamepad button)
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

        // Camera controls for axes (analog input, e.g. mouse or gamepad thumbstick)
        // These return values in [-1.0, 1.0] range.
        inputConfig->RegisterAxis("Horizontal", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
        inputConfig->RegisterAxis("Vertical", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));
        // ######################################################

        // ######################################################
        auto meshImportOptions = MeshImportOptions::Create();
        meshImportOptions->ImportNormals = true;
        meshImportOptions->ImportTangents = true;
        meshImportOptions->CpuCached = false;

        auto textureImportOptions = TextureImportOptions::Create();
        textureImportOptions->CpuCached = false;
        textureImportOptions->GenerateMips = true;

        auto textureCubeMapImportOptions = TextureImportOptions::Create();
        textureCubeMapImportOptions->CpuCached = false;
        textureCubeMapImportOptions->CubemapType = CubemapSourceType::Faces;

        _loadedMeshCube = gResourceManager().Load<Mesh>("Data/Meshes/MultiCubeMaterial/multi-cube-material.dae", meshImportOptions);
        _loadedMeshMonkey = gResourceManager().Load<Mesh>("Data/Meshes/Monkey/monkey.dae", meshImportOptions);
        _loadedTextureCube = gResourceManager().Load<Texture>("Data/Textures/Cube/diffuse.png", textureImportOptions);
        _loadedTextureMonkey = gResourceManager().Load<Texture>("Data/Textures/Monkey/diffuse.png", textureImportOptions);
        _loadedCubemapTexture = gResourceManager().Load<Texture>("Data/Textures/Skybox/cubemap.png", textureCubeMapImportOptions);

        TE_PRINT((_loadedMeshCube.GetHandleData())->data);
        TE_PRINT((_loadedMeshCube.GetHandleData())->uuid.ToString());

        TE_PRINT((_loadedMeshMonkey.GetHandleData())->data);
        TE_PRINT((_loadedMeshMonkey.GetHandleData())->uuid.ToString());

        TE_PRINT((_loadedTextureCube.GetHandleData())->data);
        TE_PRINT((_loadedTextureCube.GetHandleData())->uuid.ToString());

        TE_PRINT((_loadedTextureMonkey.GetHandleData())->data);
        TE_PRINT((_loadedTextureMonkey.GetHandleData())->uuid.ToString());

        TE_PRINT((_loadedCubemapTexture.GetHandleData())->data);
        TE_PRINT((_loadedCubemapTexture.GetHandleData())->uuid.ToString());
        // ######################################################

#if TE_PLATFORM == TE_PLATFORM_WIN32
        // ######################################################
        GPU_PROGRAM_DESC vertexShaderProgramDesc;
        {
            FileStream shaderFile("Data/Shaders/Raw/Test/Texture_VS.hlsl");

            vertexShaderProgramDesc.Type = GPT_VERTEX_PROGRAM;
            vertexShaderProgramDesc.EntryPoint = "main";
            vertexShaderProgramDesc.Language = "hlsl";
            vertexShaderProgramDesc.IncludePath = "Data/Shaders/Raw/Test/";
            vertexShaderProgramDesc.Source = shaderFile.GetAsString();
        }
        // ######################################################

        // ######################################################
        GPU_PROGRAM_DESC pixelShaderProgramDesc;
        {
            FileStream shaderFile("Data/Shaders/Raw/Test/Texture_PS.hlsl");

            pixelShaderProgramDesc.Type = GPT_PIXEL_PROGRAM;
            pixelShaderProgramDesc.EntryPoint = "main";
            pixelShaderProgramDesc.Language = "hlsl";
            pixelShaderProgramDesc.IncludePath = "Data/Shaders/Raw/Test/";
            pixelShaderProgramDesc.Source = shaderFile.GetAsString();
        }
        // ######################################################

        // ######################################################
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
        samplerDesc.MaxAnisotropy = 8;

        SPtr<BlendState> blendState = BlendState::Create(blendDesc);
        SPtr<RasterizerState> rasterizerState = RasterizerState::Create(rastDesc);
        SPtr<DepthStencilState> depthStencilState = DepthStencilState::Create(depthDesc);
        SPtr<SamplerState> samplerState = SamplerState::Create(samplerDesc);

        PIPELINE_STATE_DESC pipeDesc;
        pipeDesc.blendState = blendState;
        pipeDesc.rasterizerState = rasterizerState;
        pipeDesc.depthStencilState = depthStencilState;
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

        MaterialProperties properties;
        properties.UseDiffuseMap = true;

        _materialCube = Material::Create(_shader);
        _materialCube->SetName("Material");
        _materialCube->SetTexture("DiffuseMap", _loadedTextureCube);
        _materialCube->SetSamplerState("AnisotropicSampler", samplerState);
        _materialCube->SetProperties(properties);

        _materialMonkey = Material::Create(_shader);
        _materialMonkey->SetName("Material");
        _materialMonkey->SetTexture("DiffuseMap", _loadedTextureMonkey);
        _materialMonkey->SetSamplerState("AnisotropicSampler", samplerState);
        _materialMonkey->SetProperties(properties);

        // ######################################################

        // ######################################################
        _sceneCameraSO = SceneObject::Create("SceneCamera");
        _sceneCameraFlyer = _sceneCameraSO->AddComponent<CCameraFlyer>();
        _sceneCamera = _sceneCameraSO->AddComponent<CCamera>();
        _sceneCamera->GetViewport()->SetClearColorValue(Color(0.17f, 0.64f, 1.0f, 1.0f));
        _sceneCamera->GetViewport()->SetTarget(gCoreApplication().GetWindow());
        _sceneCamera->SetMSAACount(8);
        _sceneCamera->SetProjectionType(ProjectionType::PT_PERSPECTIVE);
        _sceneCamera->SetMain(true);
        _sceneCamera->Initialize();

        _sceneRenderableSO = SceneObject::Create("Cube");
        _renderableCube = _sceneRenderableSO->AddComponent<CRenderable>();
        _renderableCube->SetMesh(_loadedMeshCube);
        _renderableCube->SetMaterial(_materialCube);
        _renderableCube->Initialize();

        _sceneSkyboxSO = SceneObject::Create("Skybox");
        _skybox = _sceneSkyboxSO->AddComponent<CSkybox>();
        _skybox->SetTexture(_loadedCubemapTexture);
        _skybox->Initialize();

        _sceneLightSO = SceneObject::Create("Light");
        _light = _sceneLightSO->AddComponent<CLight>();
        _light->Initialize();

        _sceneCameraSO->SetPosition(Vector3(0.0f, 5.0f, 7.5f));
        _sceneCameraSO->LookAt(Vector3(0.0f, 0.0f, -3.0f));

        _sceneRenderableSO->Move(Vector3(-50.0f, 0.0f, -55.0f));

        for (INT16 i = -15; i < 16; i++)
        {
            for (INT16 j = -1; j < 16; j++)
            {
                HSceneObject sceneRenderable = SceneObject::Create("Monkey_" + ToString(i) + "_" + ToString(j));
                HRenderable renderableCube = sceneRenderable->AddComponent<CRenderable>();
                renderableCube->SetMesh(_loadedMeshMonkey);
                renderableCube->SetMaterial("Material-material", _materialMonkey);
                renderableCube->SetInstancing(true);
                renderableCube->Initialize();

                sceneRenderable->Move(Vector3((float)i * 3.0f, 0.0f, -(float)j * 3.0f));
                //sceneRenderable->SetMobility(ObjectMobility::Immovable);

                _sceneRenderablesMonkeySO.push_back(sceneRenderable);
            }
        }
        // ######################################################

        // ######################################################
        gRenderer()->BatchRenderables();
        // ######################################################

        // ######################################################
        gSceneManager().SetMainRenderTarget(gCoreApplication().GetWindow());
        // ######################################################

        // ######################################################
        auto handleButtonDown = [&](const ButtonEvent& event)
        {
            if (event.buttonCode == TE_SPACE)
            {
                _materialCube->SetTexture("DiffuseMap", _loadedTextureMonkey);
                _renderableCube->SetMaterial(_materialCube);
                TE_PRINT("SPACE");
            }
        };

        // Connect the callback to the event
        gInput().OnButtonDown.Connect(handleButtonDown);
        // ######################################################
#endif
    }

    void Application::PreShutDown()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        _pass = nullptr;
        _technique = nullptr;
#endif
    }

    void Application::PreUpdate()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        //_sceneRenderableSO->Rotate(Vector3(0.0f, 1.0f, 0.0f), Radian(1.5f * gTime().GetFrameDelta()));

        /*for (auto& so : _sceneRenderablesMonkeySO)
        {
            so->Rotate(Vector3(0.0f, 1.0f, 0.0f), Radian(2.0f * gTime().GetFrameDelta()));
        }*/
#endif
    }

    void Application::PostUpdate()
    { }
}

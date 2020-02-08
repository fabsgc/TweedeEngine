#include "TeCoreApplication.h"

#include "Error/TeConsole.h"
#include "Utility/TeTime.h"
#include "Utility/TeDynLibManager.h"
#include "Utility/TeDynLib.h"
#include "Utility/TeFileStream.h"

#include "Threading/TeThreading.h"

#include "Manager/TePluginManager.h"
#include "Manager/TeRenderAPIManager.h"
#include "Manager/TeRendererManager.h"
#include "Resources/TeResourceManager.h"
#include "RenderAPI/TeRenderStateManager.h"
#include "RenderAPI/TeGpuProgramManager.h"

#include "Input/TeInput.h"
#include "Input/TeVirtualInput.h"

#include "RenderAPI/TeRenderAPI.h"
#include "RenderAPI/TeCommonTypes.h"
#include "RenderAPI/TeRenderTexture.h"
#include "RenderAPI/TeGpuProgram.h"
#include "RenderAPI/TeVertexBuffer.h"
#include "RenderAPI/TeIndexBuffer.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "RenderAPI/TeGpuParams.h"
#include "RenderAPI/TeGpuParamBlockBuffer.h"

#include "Importer/TeImporter.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"
#include "Importer/TeShaderImportOptions.h"

#include "Renderer/TeRenderer.h"
#include "Renderer/TeCamera.h"

#include "Scene/TeSceneManager.h"
#include "Scene/TeGameObjectManager.h"

#include "CoreUtility/TeCoreObjectManager.h"

#include "Mesh/TeMesh.h"
#include "Mesh/TeMeshData.h"
#include "Mesh/TeMeshUtility.h"

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

namespace te
{
    TE_MODULE_STATIC_MEMBER(CoreApplication)

    struct PerInstanceData
    {
        Matrix4 gMatWorld;
        Matrix4 gMatInvWorld;
        Matrix4 gMatWorldNoScale;
        Matrix4 gMatInvWorldNoScale;
        Matrix4 gMatPrevWorld;
        INT32   gLayer;
    };

    CoreApplication::CoreApplication(START_UP_DESC desc)
        : _window(nullptr)
        , _startUpDesc(desc)
        , _rendererPlugin(nullptr)
        , _renderAPIPlugin(nullptr)
        , _isFrameRenderingFinished(true)
        , _runMainLoop(false)
        , _pause(false)
    { }

    CoreApplication::~CoreApplication()
    { }

    void CoreApplication::OnStartUp()
    {
        Platform::StartUp();
        Console::StartUp();
        Time::StartUp();
        DynLibManager::StartUp();
        CoreObjectManager::StartUp();
        RenderAPIManager::StartUp();
        GpuProgramManager::StartUp();
        GameObjectManager::StartUp();
        RendererManager::StartUp();
        ResourceManager::StartUp();
        SceneManager::StartUp();

        LoadPlugin(_startUpDesc.Renderer, &_rendererPlugin);
        LoadPlugin(_startUpDesc.RenderAPI, &_renderAPIPlugin);

        RenderAPIManager::Instance().Initialize(_startUpDesc.RenderAPI, _startUpDesc.WindowDesc);
        RenderAPI::Instance().Initialize();
        RenderAPI::Instance().SetDrawOperation(DOT_TRIANGLE_LIST);

        ParamBlockManager::StartUp();

        _renderer = RendererManager::Instance().Initialize(_startUpDesc.Renderer);
        _window = RenderAPI::Instance().CreateRenderWindow(_startUpDesc.WindowDesc);
        _window->Initialize();

        Input::StartUp();
        VirtualInput::StartUp();
        Importer::StartUp(); 

        for (auto& importerName : _startUpDesc.Importers)
        {
            LoadPlugin(importerName);
        }

        TestStartUp();
    }
    
    void CoreApplication::OnShutDown()
    {
        auto& i = CoreObjectManager::Instance();
        auto& j = ResourceManager::Instance();
        TestShutDown();

        Importer::ShutDown();
        VirtualInput::ShutDown();
        Input::ShutDown();
        ParamBlockManager::ShutDown();
        SceneManager::ShutDown();
        GameObjectManager::ShutDown();
        RendererManager::ShutDown();
        GpuProgramManager::ShutDown();
        ResourceManager::ShutDown();
        RenderAPIManager::ShutDown();
        CoreObjectManager::ShutDown();
        Platform::ShutDown();
        DynLibManager::ShutDown();
        Time::ShutDown();
        Console::ShutDown();
    }

    void CoreApplication::RunMainLoop()
    {
        _runMainLoop = true;

        while (_runMainLoop && !_pause)
        {
            Platform::Update();
            gTime().Update();
            gInput().Update();
            gInput().TriggerCallbacks();
            gVirtualInput().Update();
            _window->TriggerCallback();

            PreUpdate();

            gSceneManager()._update();

            for (auto& pluginUpdateFunc : _pluginUpdateFunctions)
            {
                pluginUpdateFunc.second();
            }

            PostUpdate();

            RendererManager::Instance().GetRenderer()->Update();
            RendererManager::Instance().GetRenderer()->RenderAll();

            TestRun();
        }
    }

    void CoreApplication::PreUpdate()
    { }

    void CoreApplication::PostUpdate()
    {
        DisplayFrameRate();
    }

    void CoreApplication::StopMainLoop()
    {
        _runMainLoop = false;
    }

    void CoreApplication::Pause(bool pause)
    {
        _pause = pause;
    }

    bool CoreApplication::GetPaused()
    {
        return _pause;
    }

    void CoreApplication::OnStopRequested()
    {
        StopMainLoop();
    }

    void CoreApplication::OnPauseRequested()
    {
        Pause(true);
    }

    void CoreApplication::SetFPSLimit(UINT32 limit)
    {
        if (limit > 0) _frameStep = (UINT64)1000000 / limit;
        else _frameStep = 0;
    }

    void CoreApplication::CheckFPSLimit()
    {
        if (_frameStep > 0)
        {
            UINT64 currentTime = gTime().GetTimePrecise();
            UINT64 nextFrameTime = _lastFrameTime + _frameStep;
            while (nextFrameTime > currentTime)
            {
                UINT32 waitTime = (UINT32)(nextFrameTime - currentTime);

                // If waiting for longer, sleep
                if (waitTime >= 2000)
                {
                    TE_SLEEP(waitTime / 1000);
                    currentTime = gTime().GetTimePrecise();
                }
                else
                {
                    // Otherwise we just spin, sleep timer granularity is too low and we might end up wasting a 
                    // millisecond otherwise. 
                    // Note: For mobiles where power might be more important than input latency, consider using sleep.
                    while (nextFrameTime > currentTime)
                    {
                        currentTime = gTime().GetTimePrecise();
                    }  
                }
            }

            _lastFrameTime = currentTime;
        }
    }

    void CoreApplication::DisplayFrameRate()
    {
        static int   frameCnt = 0;
        static float timeElapsed = 0.0f;
        frameCnt++;

        if (gTime().GetTime() - timeElapsed >= 1.0f)
        {
            float fps = 1.0f / gTime().GetFrameDelta();
            _window->SetTitle(_window->GetDesc().Title + " | FPS : " + ToString(fps) + " | ELAPSED : " + ToString(gTime().GetFrameDelta() * 1000));
            frameCnt = 0;
            timeElapsed += 1.0f; 
        }
    }

    void* CoreApplication::LoadPlugin(const String& pluginName, DynLib** library, void* passThrough)
    {
        DynLib* loadedLibrary = gDynLibManager().Load(pluginName);
        if (library != nullptr)
            *library = loadedLibrary;

        void* retVal = nullptr;
        if (loadedLibrary != nullptr)
        {
            if (passThrough == nullptr)
            {
                typedef void* (*LoadPluginFunc)();
                LoadPluginFunc loadPluginFunc = (LoadPluginFunc)loadedLibrary->GetSymbol("LoadPlugin");
                if (loadPluginFunc != nullptr)
                    retVal = loadPluginFunc();
            }
            else
            {
                typedef void* (*LoadPluginFunc)(void*);
                LoadPluginFunc loadPluginFunc = (LoadPluginFunc)loadedLibrary->GetSymbol("LoadPlugin");
                if (loadPluginFunc != nullptr)
                    retVal = loadPluginFunc(passThrough);
            }

            UpdatePluginFunc updatePluginFunc = (UpdatePluginFunc)loadedLibrary->GetSymbol("UpdatePlugin");

            if (updatePluginFunc != nullptr)
                _pluginUpdateFunctions[loadedLibrary] = updatePluginFunc;
        }

        return retVal;
    }

    void CoreApplication::UnloadPlugin(DynLib* library)
    {
        typedef void(*UnloadPluginFunc)();

        UnloadPluginFunc unloadPluginFunc = (UnloadPluginFunc)library->GetSymbol("UnloadPlugin");

        if (unloadPluginFunc != nullptr)
        {
            unloadPluginFunc();
        }

        _pluginUpdateFunctions.erase(library);
        gDynLibManager().Unload(library);
    }

    void CoreApplication::TestStartUp()
    {
        // ######################################################
        // Register input configuration
        // Engine allows you to use VirtualInput system which will map input device buttons and axes to arbitrary names,
        // which allows you to change input buttons without affecting the code that uses it, since the code is only
        // aware of the virtual names.  If you want more direct input, see Input class.
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

        _loadedMeshCube = gResourceManager().Load<Mesh>("Data/Meshes/multi-cube-material.dae", meshImportOptions);
        _loadedMeshMonkey = gResourceManager().Load<Mesh>("Data/Meshes/monkey.dae", meshImportOptions);
        _loadedTextureCube = gResourceManager().Load<Texture>("Data/Textures/cube.png", textureImportOptions);
        _loadedTextureMonkey = gResourceManager().Load<Texture>("Data/Textures/monkey.png", textureImportOptions);
        _loadedCubemapTexture = gResourceManager().Load<Texture>("Data/Textures/cubemap.png", textureCubeMapImportOptions);

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

#if TE_PLATFORM == TE_PLATFORM_WIN32
        // ######################################################
        GPU_PROGRAM_DESC vertexShaderProgramDesc;
        {
            FileStream shaderFile("Data/Shaders/Raw/Test/Texture_VS.hlsl");

            vertexShaderProgramDesc.Type = GPT_VERTEX_PROGRAM;
            vertexShaderProgramDesc.EntryPoint = "main";
            vertexShaderProgramDesc.Language = "hlsl";
            vertexShaderProgramDesc.Source = shaderFile.GetAsString();

            _textureVertexShader = GpuProgram::Create(vertexShaderProgramDesc);
        }
        // ######################################################

        // ######################################################
        GPU_PROGRAM_DESC pixelShaderProgramDesc;
        {
            FileStream shaderFile("Data/Shaders/Raw/Test/Texture_PS.hlsl");

            pixelShaderProgramDesc.Type = GPT_PIXEL_PROGRAM;
            pixelShaderProgramDesc.EntryPoint = "main";
            pixelShaderProgramDesc.Language = "hlsl";
            pixelShaderProgramDesc.Source = shaderFile.GetAsString();

            _texturePixelShader = GpuProgram::Create(pixelShaderProgramDesc);
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
        samplerDesc.MaxAnisotropy = 2;

        SPtr<BlendState> blendState = BlendState::Create(blendDesc);
        SPtr<RasterizerState> rasterizerState = RasterizerState::Create(rastDesc);
        SPtr<DepthStencilState> depthStencilState = DepthStencilState::Create(depthDesc);
        SPtr<SamplerState> samplerState = SamplerState::Create(samplerDesc);

        PIPELINE_STATE_DESC pipeDesc;
        pipeDesc.blendState = blendState;
        pipeDesc.rasterizerState = rasterizerState;
        pipeDesc.depthStencilState = depthStencilState;
        pipeDesc.vertexProgram = _textureVertexShader;
        pipeDesc.pixelProgram = _texturePixelShader;
        // ######################################################

        // ######################################################
        PASS_DESC passDesc;
        passDesc.BlendStateDesc = blendDesc;
        passDesc.DepthStencilStateDesc = depthDesc;
        passDesc.RasterizerStateDesc = rastDesc;
        passDesc.VertexProgramDesc = vertexShaderProgramDesc;
        passDesc.PixelProgramDesc = pixelShaderProgramDesc;

        _pass = Pass::Create(passDesc);
        _technique = Technique::Create("hlsl", { _pass });
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
        SHADER_DATA_PARAM_DESC gInstancedDesc("gInstanced", "gInstanced", GPDT_INT1);

        SHADER_DATA_PARAM_DESC gTime("gTime", "gTime", GPDT_FLOAT1);

        SHADER_DATA_PARAM_DESC gMatWorldViewProj("gMatWorldViewProj", "gMatWorldViewProj", GPDT_MATRIX_4X4);

        SHADER_DATA_PARAM_DESC gInstanceData("gInstanceData", "gInstanceData", GPDT_STRUCT);
        gInstanceData.ElementSize = sizeof(PerInstanceData);

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
        shaderDesc.AddParameter(gInstancedDesc);

        shaderDesc.AddParameter(gTime);

        shaderDesc.AddParameter(gMatWorldViewProj);

        shaderDesc.AddParameter(anisotropicSamplerDesc);
        shaderDesc.AddParameter(colorTextureDesc);

        shaderDesc.Techniques.push_back(_technique);

        _shader = Shader::Create("Texture", shaderDesc);
        _shader->SetName("Shader");
        
        _materialCube = Material::Create(_shader);
        _materialCube->SetName("Material");
        _materialCube->SetTexture("ColorTexture", _loadedTextureCube);
        _materialCube->SetSamplerState("AnisotropicSampler", samplerState);

        _materialMonkey = Material::Create(_shader);
        _materialMonkey->SetName("Material");
        _materialMonkey->SetTexture("ColorTexture", _loadedTextureMonkey);
        _materialMonkey->SetSamplerState("AnisotropicSampler", samplerState);
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

        _sceneRenderableSO->Move(Vector3(-48.0f, 0.0f, -55.0f));

        for (INT16 i = -14; i < 15; i++)
        {
            for (INT16 j = -1; j < 32; j++)
            {
                HSceneObject sceneRenderable = SceneObject::Create("Monkey_" + ToString(i) + "_" + ToString(j));
                HRenderable renderableCube = sceneRenderable->AddComponent<CRenderable>();
                renderableCube->SetMesh(_loadedMeshMonkey);
                renderableCube->SetMaterial(_materialMonkey);
                renderableCube->SetInstancing(true);
                renderableCube->Initialize();

                sceneRenderable->Move(Vector3((float)i * 3.0f, 0.0f, -(float)j * 3.0f));
                //sceneRenderable->SetMobility(ObjectMobility::Static);

                _sceneRenderablesMonkeySO.push_back(sceneRenderable);

            }
        }
        // ######################################################

        // ######################################################
        gSceneManager().SetMainRenderTarget(gCoreApplication().GetWindow());
        // ######################################################
#endif
    }

    void CoreApplication::TestRun()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        //_sceneRenderableSO->Rotate(Vector3(0.0f, 1.0f, 0.0f), Radian(1.5f * gTime().GetFrameDelta()));

        for (auto& so : _sceneRenderablesMonkeySO)
        {
            so->Rotate(Vector3(0.0f, 1.0f, 0.0f), Radian(2.0f * gTime().GetFrameDelta()));
        }
#endif
    }

    void CoreApplication::TestShutDown()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        _vertexDeclaration = nullptr;
        _indexBuffer = nullptr;
        _vertexBuffer = nullptr;
        _textureVertexShader = nullptr;
        _texturePixelShader = nullptr;
        _perObjectConstantBuffer = nullptr;
        _perCameraConstantBuffer = nullptr;
        _params = nullptr;

        _pass = nullptr;
        _technique = nullptr;
#endif
        _window = nullptr;

        _renderer = nullptr;
        _window = nullptr;
    }

    CoreApplication& gCoreApplication()
    {
        return CoreApplication::Instance();
    }

    CoreApplication* gCoreApplicationPtr()
    {
        return CoreApplication::InstancePtr();
    }
}

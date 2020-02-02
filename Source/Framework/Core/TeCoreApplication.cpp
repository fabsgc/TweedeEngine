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

namespace te
{
    TE_MODULE_STATIC_MEMBER(CoreApplication)

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
        TestShutDown();

        Importer::ShutDown();
        VirtualInput::ShutDown();
        Input::ShutDown();
        ParamBlockManager::ShutDown();
        SceneManager::ShutDown();
        RendererManager::ShutDown();
        GameObjectManager::ShutDown();
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
                {
                    retVal = loadPluginFunc();
                }
            }
            else
            {
                typedef void* (*LoadPluginFunc)(void*);

                LoadPluginFunc loadPluginFunc = (LoadPluginFunc)loadedLibrary->GetSymbol("LoadPlugin");

                if (loadPluginFunc != nullptr)
                {
                    retVal = loadPluginFunc(passThrough);
                }
            }

            UpdatePluginFunc updatePluginFunc = (UpdatePluginFunc)loadedLibrary->GetSymbol("UpdatePlugin");

            if (updatePluginFunc != nullptr)
            {
                _pluginUpdateFunctions[loadedLibrary] = updatePluginFunc;
            }  
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
        auto meshImportOptions = MeshImportOptions::Create();
        meshImportOptions->ImportNormals = true;
        meshImportOptions->ImportTangents = true;
        meshImportOptions->CpuCached = true;
        
        auto textureImportOptions = TextureImportOptions::Create();
        textureImportOptions->CpuCached = true;
        textureImportOptions->GenerateMips = true;

        _loadedMesh = gResourceManager().Load<Mesh>("Data/Meshes/multi-cube-material.dae", meshImportOptions);
        _loadTexture = gResourceManager().Load<Texture>("Data/Textures/cube.png", textureImportOptions);

        TE_PRINT((_loadedMesh.GetHandleData())->data);
        TE_PRINT((_loadedMesh.GetHandleData())->uuid.ToString());

        TE_PRINT((_loadTexture.GetHandleData())->data);
        TE_PRINT((_loadTexture.GetHandleData())->uuid.ToString());

#if TE_PLATFORM == TE_PLATFORM_WIN32
        // ######################################################
        {
            FileStream shaderFile("Data/Shaders/Raw/Test/Texture_VS.hlsl");

            GPU_PROGRAM_DESC vertexShaderProgramDesc;
            vertexShaderProgramDesc.Type = GPT_VERTEX_PROGRAM;
            vertexShaderProgramDesc.EntryPoint = "main";
            vertexShaderProgramDesc.Language = "hlsl";
            vertexShaderProgramDesc.Source = shaderFile.GetAsString();

            _textureVertexShader = GpuProgram::Create(vertexShaderProgramDesc);
        }
        // ######################################################

        // ######################################################
        {
            FileStream shaderFile("Data/Shaders/Raw/Test/Texture_PS.hlsl");

            GPU_PROGRAM_DESC pixelShaderProgramDesc;
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
        samplerDesc.MaxAnisotropy = 8;

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

        SPtr<GraphicsPipelineState> graphicsPipeline = GraphicsPipelineState::Create(pipeDesc);
        RenderAPI::Instance().SetGraphicsPipeline(graphicsPipeline);

        _params = GpuParams::Create(graphicsPipeline);
        _params->SetTexture(5, 0, _loadTexture.GetInternalPtr(), GpuParams::COMPLETE);
        _params->SetSamplerState(6, 0, samplerState);
        // ######################################################

        // ######################################################
        HSceneObject sceneCameraSO = SceneObject::Create("SceneCamera");
        _sceneCamera = sceneCameraSO->AddComponent<CCamera>();
        _sceneCamera->GetViewport()->SetClearColorValue(Color(0.17f, 0.64f, 1.0f, 1.0f));
        _sceneCamera->GetViewport()->SetTarget(gCoreApplication().GetWindow());
        _sceneCamera->SetMain(true);
        _sceneCamera->Initialize();

        HSceneObject renderableSO = SceneObject::Create("Cube");
        HRenderable renderable = renderableSO->AddComponent<CRenderable>();
        renderable->SetMesh(_loadedMesh);
        renderable->Initialize();

        HLight light = renderableSO->AddComponent<CLight>();
        light->Initialize();

        sceneCameraSO->SetPosition(Vector3(4.0f, 2.0f, 5.0f));
        sceneCameraSO->LookAt(Vector3(1.0f, 0.5f, 0.0f));

        renderableSO->Move(Vector3(1.0f, 0.0f, 0.0f));
        sceneCameraSO->Move(Vector3(1.0f, 0.0f, 0.0f));
        // ######################################################

        // ######################################################
        SPtr<GpuParamDesc> paramDesc = _textureVertexShader->GetParamDesc();

        _objectConstantBuffer = _defObjectBuffer.CreateBuffer();
        _frameConstantBuffer = _defFrameBuffer.CreateBuffer();

        UINT32 width = _window->GetProperties().Width;
        UINT32 height = _window->GetProperties().Height;

        Transform transformObject;
        _defObjectBuffer.World.Set(_objectConstantBuffer, renderableSO.GetInternalPtr()->GetWorldMatrix().Transpose());
        _params->SetParamBlockBuffer(0, 1, _objectConstantBuffer);
        // ######################################################

        // ######################################################
        gSceneManager().SetMainRenderTarget(gCoreApplication().GetWindow());
        // ######################################################

        // ######################################################
        RenderAPI& rapi = RenderAPI::Instance();
        _vertexDeclaration = _loadedMesh->GetVertexData()->vertexDeclaration;
        _vertexBuffer = _loadedMesh->GetVertexData()->GetBuffer(0);
        _indexBuffer = _loadedMesh->GetIndexBuffer();

        SPtr<MeshData> data = _loadedMesh->GetCachedData();
        UINT8* srcVertBufferData = data->GetStreamData(0);
        UINT32* srcIndexBufferData = data->GetIndices32();

        rapi.SetVertexDeclaration(_vertexDeclaration);
        rapi.SetVertexBuffers(0, &_vertexBuffer, 1);
        rapi.SetIndexBuffer(_indexBuffer);
        rapi.SetGpuParams(_params);
        // ######################################################
#endif
    }

    void CoreApplication::TestRun()
    {
#if TE_PLATFORM == TE_PLATFORM_WIN32
        RenderAPI& rapi = RenderAPI::Instance();

        rapi.SetRenderTarget(_sceneCamera->GetViewport()->GetTarget());

        SPtr<GpuParamDesc> paramDesc = _textureVertexShader->GetParamDesc();
        if (paramDesc->ParamBlocks.size() > 0)
        {
            _defFrameBuffer.ViewProj.Set(_frameConstantBuffer, _sceneCamera->GetViewMatrix().Transpose() * _sceneCamera->GetProjectionMatrix().Transpose());
            _defFrameBuffer.WorldCamera.Set(_frameConstantBuffer, _sceneCamera->_getCamera()->GetTransform().GetPosition());

            _params->SetParamBlockBuffer(GPT_VERTEX_PROGRAM, "FrameConstantBuffer", _frameConstantBuffer);
            _params->SetParamBlockBuffer(GPT_PIXEL_PROGRAM, "FrameConstantBuffer", _frameConstantBuffer);
            rapi.SetGpuParams(_params);
        }

        UINT32 clearBuffers = FBT_COLOR | FBT_DEPTH | FBT_STENCIL;
        rapi.ClearViewport(clearBuffers, _sceneCamera->GetViewport()->GetClearColorValue());
    
        UINT32 numIndices = _indexBuffer->GetProperties().GetNumIndices();
        UINT32 numVertices = _vertexBuffer->GetProperties().GetNumVertices();
        
        rapi.DrawIndexed(0, numIndices, 0, numVertices);

        RenderAPI::Instance().SwapBuffers(_sceneCamera->GetViewport()->GetTarget());
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
        _objectConstantBuffer = nullptr;
        _frameConstantBuffer = nullptr;
        _params = nullptr;
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

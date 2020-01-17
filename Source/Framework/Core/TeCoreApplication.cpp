#include "TeCoreApplication.h"

#include "Error/TeConsole.h"
#include "Utility/TeTime.h"

#include "Utility/TeDynLibManager.h"
#include "Utility/TeDynLib.h"
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
#include "Renderer/TeRenderer.h"
#include "RenderAPI/TeCommonTypes.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Image/TeTexture.h"

#include "Importer/TeImporter.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"
#include "Importer/TeShaderImportOptions.h"

#include "Renderer/TeCamera.h"
#include "Scene/TeSceneManager.h"
#include "CoreUtility/TeCoreObjectManager.h"

#include "Shader/TeShader.h"

#include "Utility/TeFileStream.h"

#include "RenderAPI/TeGpuProgram.h"

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
    {
    }

    CoreApplication::~CoreApplication()
    {
    }

    void CoreApplication::OnStartUp()
    {
        Platform::StartUp();
        Console::StartUp();
        Time::StartUp();
        DynLibManager::StartUp();

        CoreObjectManager::StartUp();
        RenderAPIManager::StartUp();
        RendererManager::StartUp();
        ResourceManager::StartUp();
        GpuProgramManager::StartUp();
        SceneManager::StartUp();
        gSceneManager().Initialize();

        LoadPlugin(_startUpDesc.Renderer, &_rendererPlugin);
        LoadPlugin(_startUpDesc.RenderAPI, &_renderAPIPlugin);

        RenderAPIManager::Instance().Initialize(_startUpDesc.RenderAPI, _startUpDesc.WindowDesc);
        RenderAPI::Instance().Initialize();
        RenderAPI::Instance().SetDrawOperation(DOT_TRIANGLE_LIST);

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
        SceneManager::ShutDown();
        GpuProgramManager::ShutDown();
        ResourceManager::ShutDown();
        RendererManager::ShutDown();
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
            //CheckFPSLimit();

            Platform::Update();
            gTime().Update();
            gInput().Update();
            _window->TriggerCallback();
            gInput().TriggerCallbacks();
            gVirtualInput().Update();

            PreUpdate();

            for (auto& pluginUpdateFunc : _pluginUpdateFunctions)
            {
                pluginUpdateFunc.second();
            }

            _renderer->Update();

            PostUpdate();

            RendererManager::Instance().GetRenderer()->RenderAll();

            TestRun();
        }
    }

    void CoreApplication::PreUpdate()
    {
        // Do nothing
    }

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
        if (limit > 0)
        {
            _frameStep = (UINT64)1000000 / limit;
        } 
        else
        {
            _frameStep = 0;
        } 
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
        auto resizedCallback = []()
        {
            TE_DEBUG("Window was resized.", __FILE__, __LINE__);
        };

        _window->OnResized.Connect(resizedCallback);
        // ######################################################

        // ######################################################
        SPtr<InputConfiguration> inputConfig = gVirtualInput().GetConfiguration();

        inputConfig->RegisterButton("Select All", TE_A);
        inputConfig->RegisterButton("Unwrap", TE_U);
        inputConfig->RegisterButton("Forward", TE_UP);
        // ######################################################

        // ######################################################
        auto handleButtonHeld = [&](const VirtualButton& btn, UINT32 deviceIdx)
        {
            TE_PRINT(btn.ButtonIdentifier);
        };

        gVirtualInput().OnButtonDown.Connect(handleButtonHeld);
        // ######################################################

        // ######################################################
        // Callback method that triggers when any button is pressed
        auto handleButtonDown = [&](const ButtonEvent& event)
        {
            // If user presses space, "jump"
            if (event.buttonCode == TE_A)
            {
                TE_PRINT("A");
            }
        };

        // Connect the callback to the event
        gInput().OnButtonDown.Connect(handleButtonDown);

        auto handlePointerMove = [&](const PointerEvent& event)
        {
            Vector3 position(TeZero);
            Vector3 delta(TeZero);

            position.x = (float)event.screenPos.x;
            position.y = (float)event.screenPos.y;

            delta.x = (float)event.delta.x;
            delta.y = (float)event.delta.y;

            //TE_PRINT("Mouse position : " + ToString(position.x) + "/" + ToString(position.y));
            //TE_PRINT("Mouse delta : " + ToString(delta.x) + "/" + ToString(delta.y));
        };
        // ######################################################

        // ######################################################
        gInput().OnPointerMoved.Connect(handlePointerMove);

        auto handlePointerRelativeMove = [&](const Vector2I& delta)
        {
            //TE_PRINT("Mouse absolute delta : " + ToString(delta.x) + "/" + ToString(delta.y));
        };

        gInput().OnPointerRelativeMoved.Connect(handlePointerRelativeMove);
        // ######################################################

        // ######################################################
        VIRTUAL_AXIS_DESC desc;
        desc.DeadZone = 0.1f;
        desc.Normalize = true;
        desc.Type = (int)InputAxis::RightStickX;

        inputConfig->RegisterAxis("LookLeftRight", desc);
        // ######################################################

        // ######################################################
        auto meshImportOptions = MeshImportOptions::Create();
        meshImportOptions->ImportNormals = false;
        auto textureImportOptions = TextureImportOptions::Create();
        textureImportOptions->CpuCached = true;
        auto shaderImportOptions = ShaderImportOptions::Create();

        HMesh loadedMesh = gResourceManager().Load<Mesh>("Data/Meshes/cube.obj", meshImportOptions);
        HTexture loadTexture = gResourceManager().Load<Texture>("Data/Textures/default.png", textureImportOptions);
        HShader loadShader = gResourceManager().Load<Shader>("Data/Shaders/default.shader", shaderImportOptions);

        TE_PRINT((loadedMesh.GetHandleData())->data);
        TE_PRINT((loadedMesh.GetHandleData())->uuid.ToString());

        TE_PRINT((loadTexture.GetHandleData())->data);
        TE_PRINT((loadTexture.GetHandleData())->uuid.ToString());

        TE_PRINT((loadShader.GetHandleData())->data);
        TE_PRINT((loadShader.GetHandleData())->uuid.ToString());
        // ######################################################

        // ######################################################
        _camera = Camera::Create();
        _camera->SetRenderTarget(gCoreApplication().GetWindow());
        _camera->GetViewport()->SetClearColorValue(Color(0.6f, 0.0f, 0.2f, 1.0f));
        _camera->GetViewport()->SetArea(Rect2(0.5, 0.5, 0.5, 0.5));
        _camera->SetMain(true);
        // ######################################################

        // ######################################################
        BLEND_STATE_DESC blendDesc;
        RASTERIZER_STATE_DESC rastDesc;
        DEPTH_STENCIL_STATE_DESC depthDesc;
        SAMPLER_STATE_DESC samplerDesc;

        rastDesc.polygonMode = PM_WIREFRAME; // Draw wireframe instead of solid
        rastDesc.cullMode = CULL_NONE; // Disable blackface culling

        SPtr<BlendState> blendState = BlendState::Create(blendDesc);
        SPtr<RasterizerState> rasterizerState = RasterizerState::Create(rastDesc);
        SPtr<DepthStencilState> depthStencilState = DepthStencilState::Create(depthDesc);
        SPtr<SamplerState> samplerState = SamplerState::Create(samplerDesc);

        PIPELINE_STATE_DESC pipeDesc;
        pipeDesc.blendState = blendState;
        pipeDesc.rasterizerState = rasterizerState;
        pipeDesc.depthStencilState = depthStencilState;

        SPtr<GraphicsPipelineState> graphicsPipeline = GraphicsPipelineState::Create(pipeDesc);

        RenderAPI::Instance().SetGraphicsPipeline(graphicsPipeline);
        // ######################################################

        // ######################################################
        FileStream file1("Data/Textures/default.png");
        FileStream file2("Data\\Textures\\default.png");
        // ######################################################

        // ######################################################
        // Create a 960x480 texture with 32-bit RGBA format
        TEXTURE_DESC targetColorDesc;
        targetColorDesc.Type = TEX_TYPE_2D;
        targetColorDesc.Width = 960;
        targetColorDesc.Height = 480;
        targetColorDesc.Format = PF_RGBA8;
        targetColorDesc.Usage = TU_RENDERTARGET | TU_CPUCACHED;

        HTexture color = Texture::Create(targetColorDesc);

        // Create a 960x480 texture with a 32-bit depth-stencil format
        TEXTURE_DESC targetDepthDesc;
        targetDepthDesc.Type = TEX_TYPE_2D;
        targetDepthDesc.Width = 960;
        targetDepthDesc.Height = 480;
        targetDepthDesc.Format = PF_RGBA8;
        targetDepthDesc.Usage = TU_DEPTHSTENCIL | TU_CPUCACHED;

        HTexture depthStencil = Texture::Create(targetDepthDesc);

        RENDER_TEXTURE_DESC renderTextureDesc;
        renderTextureDesc.ColorSurfaces[0].Tex = color.GetInternalPtr();
        renderTextureDesc.ColorSurfaces[0].Face = 0;
        renderTextureDesc.ColorSurfaces[0].MipLevel = 0;

        renderTextureDesc.DepthStencilSurface.Tex = depthStencil.GetInternalPtr();
        renderTextureDesc.DepthStencilSurface.Face = 0;
        renderTextureDesc.DepthStencilSurface.MipLevel = 0;

        SPtr<RenderTexture> renderTexture = RenderTexture::Create(renderTextureDesc);
        // ######################################################

        // ######################################################
        _cameraHidden = Camera::Create();
        _cameraHidden->SetRenderTarget(renderTexture);
        _cameraHidden->GetViewport()->SetClearColorValue(Color(0.2f, 0.0f, 0.2f, 1.0f));
        _cameraHidden->SetMain(false);

        SPtr<PixelData> pixelData = loadTexture->GetProperties().AllocBuffer(0, 0);
        loadTexture->ReadCachedData(*pixelData);
        Color pixelColor = pixelData->GetColorAt(50, 50);
        // ######################################################

        // ######################################################
        FileStream vertexShaderFile("Data/Shaders/Raw/Test/VS.hlsl");

        GPU_PROGRAM_DESC vertexShaderProgramDesc;
        vertexShaderProgramDesc.Type = GPT_VERTEX_PROGRAM;
        vertexShaderProgramDesc.EntryPoint = "main";
        vertexShaderProgramDesc.Language = "hlsl";
        vertexShaderProgramDesc.Source = vertexShaderFile.GetAsString();

        _vertexShader = GpuProgram::Create(vertexShaderProgramDesc);
        // ######################################################

        // ######################################################
        FileStream pixelShaderFile("Data/Shaders/Raw/Test/PS.hlsl");

        GPU_PROGRAM_DESC pixelShaderProgramDesc;
        pixelShaderProgramDesc.Type = GPT_PIXEL_PROGRAM;
        pixelShaderProgramDesc.EntryPoint = "main";
        pixelShaderProgramDesc.Language = "hlsl";
        pixelShaderProgramDesc.Source = pixelShaderFile.GetAsString();

        _pixelShader = GpuProgram::Create(pixelShaderProgramDesc);
        // ######################################################

        CoreObjectManager& i = CoreObjectManager::Instance();
    }

    void CoreApplication::TestRun()
    {
        SPtr<Texture> renderTexture = (static_cast<RenderTexture&>(*(_cameraHidden->GetViewport()->GetTarget()))).GetColorTexture(0);
        SPtr<PixelData> pixelData = renderTexture->GetProperties().AllocBuffer(0, 0);
        renderTexture->ReadData(*pixelData);
        Color pixelColor = pixelData->GetColorAt(50, 50);
    }

    void CoreApplication::TestShutDown()
    {
        _vertexShader->Destroy();
        _vertexShader = nullptr;

        _pixelShader->Destroy();
        _pixelShader = nullptr;

        _window->Destroy();
        _window = nullptr;

        _camera->Destroy();
        _camera = nullptr;

        _cameraHidden->Destroy();
        _cameraHidden = nullptr;

        _renderer.reset();
        _window.reset();
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

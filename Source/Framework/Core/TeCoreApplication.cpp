#include "TeCoreApplication.h"

#include "Utility/TeDynLibManager.h"
#include "Utility/TeDynLib.h"
#include "Threading/TeThreading.h"
#include "Manager/TePluginManager.h"
#include "Manager/TeRenderAPIManager.h"
#include "Manager/TeRendererManager.h"
#include "Error/TeConsole.h"
#include "Utility/TeTime.h"
#include "Input/TeInput.h"
#include "Input/TeVirtualInput.h"
#include "RenderAPI/TeRenderAPI.h"
#include "Renderer/TeRenderer.h"
#include "Importer/TeImporter.h"
#include "Resources/TeResourceManager.h"
#include "Renderer/TeCamera.h"
#include "Scene/TeSceneManager.h"
#include "CoreUtility/TeCoreObjectManager.h"
#include "RenderAPI/TeRenderStateManager.h"
#include "Importer/TeMeshImportOptions.h"
#include "Importer/TeTextureImportOptions.h"

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

        SPtr<InputConfiguration> inputConfig = gVirtualInput().GetConfiguration();

        inputConfig->RegisterButton("Select All", TE_A);
        inputConfig->RegisterButton("Unwrap", TE_U);
        inputConfig->RegisterButton("Forward", TE_UP);

        auto handleButtonHeld = [&](const VirtualButton& btn, UINT32 deviceIdx)
        {
            TE_PRINT(btn.ButtonIdentifier);
        };

        gVirtualInput().OnButtonDown.Connect(handleButtonHeld);

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

        gInput().OnPointerMoved.Connect(handlePointerMove);

        auto handlePointerRelativeMove = [&](const Vector2I& delta)
        {
            //TE_PRINT("Mouse absolute delta : " + ToString(delta.x) + "/" + ToString(delta.y));
        };

        gInput().OnPointerRelativeMoved.Connect(handlePointerRelativeMove);

        VIRTUAL_AXIS_DESC desc;
        desc.DeadZone = 0.1f;
        desc.Normalize = true;
        desc.Type = (int)InputAxis::RightStickX;

        inputConfig->RegisterAxis("LookLeftRight", desc);

        Importer::StartUp();
        
        for (auto& importerName : _startUpDesc.Importers)
        {
            LoadPlugin(importerName);
        }

        auto meshImportOptions = MeshImportOptions::Create();
        meshImportOptions->ImportNormals = false;
        auto textureImportOptions = TextureImportOptions::Create();

        HMesh loadedMesh = gResourceManager().Load<Mesh>("Data/Mesh/cube.obj", meshImportOptions);
        HTexture loadTexture = gResourceManager().Load<Texture>("Data/Texture/default.png", textureImportOptions);

        TE_PRINT((loadedMesh.GetHandleData())->data);
        TE_PRINT((loadedMesh.GetHandleData())->uuid.ToString());

        TE_PRINT((loadTexture.GetHandleData())->data);
        TE_PRINT((loadTexture.GetHandleData())->uuid.ToString());

        _camera = Camera::Create();
        _camera->SetRenderTarget(gCoreApplication().GetWindow());
        _camera->SetMain(true);

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

    }
    
    void CoreApplication::OnShutDown()
    {
        _camera->Destroy();

        _renderer.reset();
        _window.reset();
        
        Importer::ShutDown();
        VirtualInput::ShutDown();
        Input::ShutDown();
        SceneManager::ShutDown();
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
            CheckFPSLimit();

            Platform::Update();
            gTime().Update();
            gInput().Update();
            _window->TriggerCallback();
            gInput().TriggerCallbacks();
            gVirtualInput().Update();

            PreUpdate();

            VirtualAxis lookLeftRightAxis("LookLeftRight");

            //float value = gVirtualInput().GetAxisValue(lookLeftRightAxis);

            //if(value != 0.0f)
            //{
            //    TE_PRINT(value)
            //}

            for (auto& pluginUpdateFunc : _pluginUpdateFunctions)
            {
                pluginUpdateFunc.second();
            }

            _renderer->Update();

            PostUpdate();

            RendererManager::Instance().GetRenderer()->RenderAll();
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

    void CoreApplication::StartUpRenderer()
    {
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

    CoreApplication& gCoreApplication()
    {
        return CoreApplication::Instance();
    }

    CoreApplication* gCoreApplicationPtr()
    {
        return CoreApplication::InstancePtr();
    }
}

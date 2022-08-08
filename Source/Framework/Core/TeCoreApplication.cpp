#include "TeCoreApplication.h"

#include "Error/TeConsole.h"
#include "Utility/TeTime.h"
#include "Utility/TeDynLibManager.h"
#include "Utility/TeDynLib.h"
#include "Threading/TeTaskScheduler.h"

#include "Manager/TePluginManager.h"
#include "Manager/TeRenderAPIManager.h"
#include "Manager/TeRendererManager.h"
#include "Manager/TeGuiManager.h"
#include "Manager/TeRenderDocManager.h"
#include "Resources/TeResourceManager.h"
#include "RenderAPI/TeRenderStateManager.h"
#include "RenderAPI/TeGpuProgramManager.h"
#include "Renderer/TeParamBlocks.h"
#include "Resources/TeBuiltinResources.h"
#include "Scene/TeSceneManager.h"
#include "Scene/TeGameObjectManager.h"
#include "CoreUtility/TeCoreObjectManager.h"
#include "Renderer/TeRendererMaterialManager.h"
#include "Animation/TeAnimationManager.h"
#include "Scripting/TeScriptManager.h"
#include "Audio/TeAudioManager.h"
#include "Physics/TePhysicsManager.h"

#include "Audio/TeAudio.h"
#include "Physics/TePhysics.h"

#include "Input/TeInput.h"
#include "Input/TeVirtualInput.h"

#include "RenderAPI/TeRenderAPI.h"
#include "Importer/TeImporter.h"
#include "Renderer/TeRenderer.h"
#include "Profiling/TeProfilerGPU.h"

#include "Gui/TeGuiAPI.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(CoreApplication)

    CoreApplication::CoreApplication(START_UP_DESC desc)
        : _window(nullptr)
        , _startUpDesc(desc)
        , _rendererPlugin(nullptr)
        , _renderAPIPlugin(nullptr)
        , _guiPlugin(nullptr)
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
        TaskScheduler::StartUp();
        DynLibManager::StartUp();
        CoreObjectManager::StartUp();
        ProfilerGPU::StartUp();
        RenderDocManager::StartUp();
        RenderAPIManager::StartUp();
        GuiManager::StartUp();
        GpuProgramManager::StartUp();
        GameObjectManager::StartUp();
        RendererManager::StartUp();
        ResourceManager::StartUp();
        ScriptManager::StartUp();

        LoadPlugin(_startUpDesc.Renderer, &_rendererPlugin);
        LoadPlugin(_startUpDesc.RenderAPI, &_renderAPIPlugin);
        LoadPlugin(_startUpDesc.Gui, &_guiPlugin);

        RenderAPIManager::Instance().Initialize(_startUpDesc.RenderAPI, _startUpDesc.WindowDesc);
        RenderAPI::Instance().Initialize();
        RenderAPI::Instance().SetDrawOperation(DOT_TRIANGLE_LIST);
        _window = RenderAPI::Instance().CreateRenderWindow(_startUpDesc.WindowDesc);
        TE_ASSERT_ERROR(_window.get(), "Failed to create renderer");

        ParamBlockManager::StartUp();

        _renderer = RendererManager::Instance().Initialize(_startUpDesc.Renderer, "Default");
        TE_ASSERT_ERROR(_renderer.get(), "Failed to create renderer");

        Importer::StartUp();
        for (auto& importerName : _startUpDesc.Importers)
            LoadPlugin(importerName);

        BuiltinResources::StartUp();
        PhysicsManager::StartUp(_startUpDesc.Physics);
        RendererMaterialManager::StartUp();
        SceneManager::StartUp();
        Input::StartUp();
        VirtualInput::StartUp();

        _gui = GuiManager::Instance().Initialize(_startUpDesc.Gui);
        TE_ASSERT_ERROR(_gui.get(), "Failed to create gui");

        _window->InitializeGui();
        _frameData = te_shared_ptr_new<FrameData>();

        AudioManager::StartUp(_startUpDesc.Audio);
        AnimationManager::StartUp();

#if TE_PLATFORM == TE_PLATFORM_WIN32
        Platform::SetIcon(gBuiltinResources().GetFrameworkIcon());
#endif

        PostStartUp();
    }
    
    void CoreApplication::OnShutDown()
    {
        PreShutDown();

        _window = nullptr;
        _renderer = nullptr;

        TaskScheduler::ShutDown();
        Importer::ShutDown();
        VirtualInput::ShutDown();
        Input::ShutDown();
        ParamBlockManager::ShutDown();
        SceneManager::ShutDown();
        ScriptManager::ShutDown();
        AnimationManager::ShutDown();
        GameObjectManager::ShutDown();
        RendererManager::ShutDown();
        ResourceManager::ShutDown();
        RendererMaterialManager::ShutDown();
        PhysicsManager::ShutDown();
        BuiltinResources::ShutDown();
        AudioManager::ShutDown();
        GuiManager::ShutDown();
        RenderAPIManager::ShutDown();
        RenderDocManager::ShutDown();
        GpuProgramManager::ShutDown();
        CoreObjectManager::ShutDown();
        Platform::ShutDown();
        DynLibManager::ShutDown();
        Time::ShutDown();
        Console::ShutDown();
    }

    void CoreApplication::RunMainLoop()
    {
        _runMainLoop = true;

        while (_runMainLoop)
        {
            Platform::Update();
            gTime().Update();
            gInput().Update();
            gInput().TriggerCallbacks();
            gVirtualInput().Update();
            _window->TriggerCallback();

            if(_pause)
            {
                TE_SLEEP(100);
                continue;
            }

            gScriptManager().PreUpdate();
            PreUpdate();

            gScriptManager().Update();
            gSceneManager().Update();
            gAudio().Update();
            gPhysics().Update();

            for (auto& pluginUpdateFunc : _pluginUpdateFunctions)
            {
                pluginUpdateFunc.second();
            }

            gScriptManager().PostUpdate();
            PostUpdate();

            _frameData->Animation = AnimationManager::Instance().Update();

            DisplayFrameRate();

            gRenderer()->Update();
            gRenderer()->RenderAll(*_frameData);

            gScriptManager().PostRender();
            PostRender();
        }
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
                        currentTime = gTime().GetTimePrecise();
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
            unloadPluginFunc();

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

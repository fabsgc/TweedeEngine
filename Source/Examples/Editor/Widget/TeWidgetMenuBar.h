#pragma once

#include "TeCorePrerequisites.h"
#include "Input/TeVirtualInput.h"
#include "TeWidget.h"

namespace te
{
    class ImGuiFileBrowser;

    class WidgetMenuBar : public Widget
    {
    public:
        static const String NEW_BINDING;
        static const String OPEN_BINDING;
        static const String SAVE_BINDING;
        static const String SAVE_AS_BINDING;
        static const String QUIT_BINDING;
        static const String LOAD_RESOURCE_BINDING;

    public:
        WidgetMenuBar();
        virtual ~WidgetMenuBar();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:
        void ShowAboutWindow();
        void ShowNewProject();
        void ShowOpenProject();
        void ShowSaveProject();
        void ShowLoadResource();
        void SaveProject();
        void SaveProjectAs();
        void Quit();

    protected:
        struct WidgetMenubarSettings
        {
            bool AboutWindow = false;
            bool NewProject = false;
            bool OpenProject = false;
            bool SaveProject = false;
            bool LoadResource = false;
        };

    protected:
        WidgetMenubarSettings _settings;
        ImGuiFileBrowser& _fileBrowser;

        VirtualButton _newBtn;
        VirtualButton _openBtn;
        VirtualButton _saveBtn;
        VirtualButton _saveAsBtn;
        VirtualButton _quitBtn;
        VirtualButton _loadResource;
    };
}

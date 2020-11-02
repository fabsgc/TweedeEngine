#pragma once

#include "TeCorePrerequisites.h"
#include "Input/TeVirtualInput.h"
#include "TeWidget.h"
#include "../ImGuiExt/TeImGuiFileBrowser.h"

namespace te
{
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
        ~WidgetMenuBar();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:
        void ShowAboutWindow();
        void ShowOpen();
        void ShowSave();
        void ShowLoad();
        void Quit();
        void Save();

    protected:
        struct WidgetMenubarSettings
        {
            bool AboutWindow = false;
            bool Open = false;
            bool Save = false;
            bool Load = false;
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

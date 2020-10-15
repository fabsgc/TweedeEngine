#pragma once

#include "TeCorePrerequisites.h"
#include "Widget/TeWidget.h"
#include "Utility/TeModule.h"
#include <vector>
#include <memory>

namespace te
{
    class Editor : public Module<Editor>
    {
    public:
        TE_MODULE_STATIC_HEADER_MEMBER(Editor)

        Editor();
        ~Editor();

        /** @copydoc Module::OnStartUp */
        void OnStartUp() override;

        /** @copydoc Module::OnShutDown */
        void OnShutDown() override;

        /** Called every frame. */
        void Update();

        Widget* GetWidget(Widget::WidgetType type)
        {
            for (auto& widget : _widgets)
            {
                if (widget->GetType() == type)
                    return widget.get();
            }

            return nullptr;
        }

    protected:
        struct EditorSettings
        {
            SPtr<Widget> WMenuBar = nullptr;
            SPtr<Widget> WToolbar = nullptr;
            SPtr<Widget> WProject = nullptr;
            const char* EditorName = "Editor";
            bool Show = true;
        };

    protected:
        void InitializeGui();
        void ApplyStyleGui() const;
        void BeginGui();
        void EndGui();

    protected:
        std::vector<SPtr<Widget>> _widgets;
        EditorSettings _settings;
        bool _editorBegun;
    };
}

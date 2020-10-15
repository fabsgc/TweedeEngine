#pragma once

#include "TeCorePrerequisites.h"
#include "Widget/TeWidget.h"
#include "Utility/TeModule.h"
#include <vector>

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

        template<typename T>
        T* GetWidget()
        {
            for (const auto& widget : _widgets)
            {
                if (T* widget = dynamic_cast<T*>(widget.get()))
                {
                    return widget;
                }
            }

            return nullptr;
        }

    protected:
        struct EditorSettings
        {
            SPtr<Widget> WidgetMenuBar = nullptr;
            SPtr<Widget> WidgetToolbar = nullptr;
            SPtr<Widget> WidgetProject = nullptr;
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

#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResourceListener.h"
#include "TeWidget.h"

namespace te
{
    class WidgetShaders : public Widget, public ResourceListener
    {
    public:
        WidgetShaders();
        virtual ~WidgetShaders();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override {};

    private:
        void Build();

        /** @copydoc ResourceListener::OnResourceModified */
        void OnResourceModified(const HResource& resource) override;

        /** @copydoc ResourceListener::OnResourceDestroyed */
        void OnResourceDestroyed(const UUID& uuid, CoreType type) override;

    private:
        HShader _currentShader;
        bool _showTechniques;
    };
}

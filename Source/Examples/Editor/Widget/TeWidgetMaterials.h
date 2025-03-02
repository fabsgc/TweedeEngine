#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"
#include "Resources/TeResourceListener.h"

namespace te
{
    class ImGuiFileBrowser;
    class MaterialsPreview;

    class WidgetMaterials : public Widget, public ResourceListener
    {
    public:
        WidgetMaterials();
        virtual ~WidgetMaterials();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:
        bool ShowLoadedTexture();

        /** @copydoc ResourceListener::OnResourceModified */
        void OnResourceModified(const HResource& resource) override;

        /** @copydoc ResourceListener::OnResourceDestroyed */
        void OnResourceDestroyed(const UUID& uuid, CoreType type) override;

    protected:
        HMaterial _currentMaterial;
        UINT32 _materialCreationCounter;

        // Data used to set the correct texture while loading
        bool _loadTexture;
        String _loadTextureName;
        bool* _loadTextureUsed;

        ImGuiFileBrowser& _fileBrowser;
        MaterialsPreview& _materialsPreview;
    };
}

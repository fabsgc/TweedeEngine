#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"
#include "../ImGuiExt/TeImGuiFileBrowser.h"

namespace te
{
    class WidgetMaterials : public Widget
    {
    public:
        WidgetMaterials();
        ~WidgetMaterials();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:
        bool ShowLoadedTexture();
        void DeleteMaterial(SPtr<Material>& material, const UUID& uuid);

    protected:
        SPtr<Material> _currentMaterial;
        UINT32 _materialCreationCounter;
        
        // Data used to set the correct texture while loading
        bool _loadTexture;
        String _loadTextureName;
        bool* _loadTextureUsed;

        ImGuiFileBrowser& _fileBrowser;
    };
}

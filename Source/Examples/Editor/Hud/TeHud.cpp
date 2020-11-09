#include "TeHud.h"

#include "../TeEditor.h"
#include "RenderAPI/TeRenderTarget.h"
#include "Scene/TeSceneObject.h"
#include "Renderer/TeCamera.h"
#include "TeHudMat.h"

namespace te
{ 
    void Hud::Initialize()
    {
        _material = HudMat::Get();
    }

    void Hud::Render(const HCamera& camera, const HSceneObject& root)
    {
        const SPtr<RenderSettings>& settings = camera->GetRenderSettings();
        if (settings->OutputType != RenderOutputType::Final)
            return;

        SPtr<RenderTarget> target = camera->GetViewport()->GetTarget();

        TE_PRINT("HUD RENDER");
    }
}

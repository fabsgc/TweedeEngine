#include "TeHud.h"
#include "TeHudMat.h"

namespace te
{ 
    void Hud::Initialize()
    {
        _material = HudMat::Get();
    }

    void Hud::Render()
    { }
}

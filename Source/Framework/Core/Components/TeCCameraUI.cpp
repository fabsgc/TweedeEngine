#include "TeCCameraUI.h"
#include "Scene/TeSceneObject.h"
#include "Math/TeVector3.h"
#include "Math/TeVector2I.h"
#include "Utility/TeTime.h"
#include "Math/TeMath.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCCamera.h"
#include "Platform/TePlatform.h"
#include "TeCoreApplication.h"

namespace te
{
    const float CCameraUI::MOVE_SPEED = 25.0f;
    const float CCameraUI::ROTATION_SPEED = 2.0f;

    CCameraUI::CCameraUI(const HSceneObject& parent)
        : Component(parent, TID_CCameraUI)
    {
        // Set a name for the component, so we can find it later if needed
        SetName("CCameraUI");

        // Get handles for key bindings. Actual keys attached to these bindings will be registered during app start-up.
        _zoomCam = VirtualButton("ZoomCam");
        _moveCam = VirtualButton("MoveCam");
        _rotateCam = VirtualButton("RotateCam");
        _horizontalAxis = VirtualAxis("Horizontal");
        _verticalAxis = VirtualAxis("Vertical");
    }

    void CCameraUI::Update()
    { }
}

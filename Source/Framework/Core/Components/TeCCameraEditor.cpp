#include "TeCCameraEditor.h"
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
    const float CCameraEditor::MOVE_SPEED = 25.0f;
    const float CCameraEditor::ROTATION_SPEED = 2.0f;

    CCameraEditor::CCameraEditor(const HSceneObject& parent)
        : Component(parent, TID_CCameraEditor)
    {
        // Set a name for the component, so we can find it later if needed
        SetName("CCameraEditor");

        // Get handles for key bindings. Actual keys attached to these bindings will be registered during app start-up.
        _zoomCam = VirtualButton("ZoomCam");
        _moveCam = VirtualButton("MoveCam");
        _rotateCam = VirtualButton("RotateCam");
        _horizontalAxis = VirtualAxis("Horizontal");
        _verticalAxis = VirtualAxis("Vertical");
    }

    void CCameraEditor::Update()
    { }
}

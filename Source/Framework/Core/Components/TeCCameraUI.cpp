#include "TeCCameraUI.h"

#include "Math/TeMath.h"
#include "Math/TeVector3.h"
#include "Math/TeVector2I.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCCamera.h"
#include "Platform/TePlatform.h"
#include "TeCoreApplication.h"
#include "Utility/TeTime.h"
#include "Input/TeInput.h"
#include "Gui/TeGuiAPI.h"

namespace te
{
    const String CCameraUI::ROTATE_BINDING = "SceneRotate";
    const String CCameraUI::MOVE_BINDING = "SceneMove";
    const String CCameraUI::ZOOM_BINDING = "SceneZoom";
    const String CCameraUI::HORIZONTAL_AXIS_BINDING = "SceneHorizontal";
    const String CCameraUI::VERTICAL_AXIS_BINDING = "SceneVertical";
    const String CCameraUI::SCROLL_AXIS_BINDING = "SceneScroll";

    const float CCameraUI::MOVE_SPEED = 0.4f;
    const float CCameraUI::ZOOM_SPEED = 32.0f;
    const float CCameraUI::SCROLL_SPEED = 6.0f;
    const float CCameraUI::ROTATIONAL_SPEED = 8.0f;
    const float CCameraUI::TOP_SCROLL_SPEED = 16.0f;

    CCameraUI::CCameraUI(const HSceneObject& parent)
        : Component(parent, TID_CCameraUI)
        , _target(Vector3::ZERO)
        , _inputEnabled(false)
        , _lastHideCursorState(false)
    {
        // Set a name for the component, so we can find it later if needed
        SetName("CCameraUI");

        _camera = static_object_cast<CCamera>(_parent->GetComponent(TID_CCamera)->GetHandle());

        _rotateBtn = VirtualButton(ROTATE_BINDING);
        _moveBtn = VirtualButton(MOVE_BINDING);
        _zoomBtn = VirtualButton(ZOOM_BINDING);

        _horizontalAxis = VirtualAxis(HORIZONTAL_AXIS_BINDING);
        _verticalAxis = VirtualAxis(VERTICAL_AXIS_BINDING);
        _scrollAxis = VirtualAxis(SCROLL_AXIS_BINDING);
    }

    void CCameraUI::Update()
    { 
        bool isOrtographic = _camera->GetProjectionType() == ProjectionType::PT_ORTHOGRAPHIC;

        bool camRotating = gVirtualInput().IsButtonHeld(_rotateBtn);
        bool camMoving = gVirtualInput().IsButtonHeld(_moveBtn);
        bool camZooming = gVirtualInput().IsButtonHeld(_zoomBtn);

        bool hideCursor = camRotating;

        if (!_inputEnabled)
        {
            if(_lastHideCursorState == hideCursor) Platform::ShowCursor();
            return;
        }

        if (hideCursor != _lastHideCursorState)
        {
            if (hideCursor)
                Platform::HideCursor();
            else
            {
                UINT32 width = gCoreApplication().GetWindow()->GetProperties().Width;
                UINT32 height = gCoreApplication().GetWindow()->GetProperties().Height;
                UINT32 left = gCoreApplication().GetWindow()->GetProperties().Left;
                UINT32 top = gCoreApplication().GetWindow()->GetProperties().Top;

                Platform::ShowCursor();
                Platform::SetCursorPosition(Vector2I(width / 2 + left, height / 2 + top));
            }

            _lastHideCursorState = hideCursor;
        }

        float frameDelta = gTime().GetFrameDelta();

        auto scrolling = [&](const float& scrollAmount, const float& speed) {
            if (!isOrtographic)
            {
                SO()->Move(SO()->GetLocalTransform().GetForward() * scrollAmount * speed * frameDelta);
            }
            else
            {
                float orthoHeight = Math::Max(1.0f, _camera->GetOrthoWindowHeight() - scrollAmount * frameDelta);
                _camera->SetOrthoWindowHeight(orthoHeight);
            }
        };

        if (camRotating)
        {
            if (camZooming)
            {
                scrolling(gVirtualInput().GetAxisValue(_verticalAxis), ZOOM_SPEED);
            }
            else if (camMoving)
            {
                float horzValue = gVirtualInput().GetAxisValue(_horizontalAxis);
                float vertValue = gVirtualInput().GetAxisValue(_verticalAxis);

                Vector3 direction = Vector3(horzValue, -vertValue, 0.0f);
                direction = SO()->GetLocalTransform().GetRotation().Rotate(direction);

                SO()->Move(direction * MOVE_SPEED);
                _target += direction * MOVE_SPEED;
            }
            else
            {
                float horzValue = gVirtualInput().GetAxisValue(_horizontalAxis);
                float vertValue = gVirtualInput().GetAxisValue(_verticalAxis);

                const Transform& tfrm = SO()->GetLocalTransform();

                SO()->RotateAround(_target, tfrm.GetRight(), Radian(Degree(Math::Clamp(vertValue * ROTATIONAL_SPEED, -90.0f, 90.f))));
                SO()->RotateAround(_target, Vector3::UNIT_Y, Radian(Degree(Math::Clamp(horzValue * ROTATIONAL_SPEED, -90.0f, 90.f))));
            }
        }
        else
        {
            float scrollAmount = Math::Clamp(gVirtualInput().GetAxisValue(_scrollAxis), -TOP_SCROLL_SPEED, TOP_SCROLL_SPEED);

            if (fabs(scrollAmount) > 1.0f)
                scrolling(scrollAmount, SCROLL_SPEED);
        }
    }

    void CCameraUI::EnableInput(bool enable)
    {
        _inputEnabled = enable;
    }

    void CCameraUI::SetTarget(Vector3 target)
    {
        _target = target;
    }
}

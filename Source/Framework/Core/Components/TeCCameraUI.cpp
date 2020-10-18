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
    const String CCameraUI::MOVE_FORWARD_BINDING = "SceneForward";
    const String CCameraUI::MOVE_LEFT_BINDING = "SceneLeft";
    const String CCameraUI::MOVE_RIGHT_BINDING = "SceneRight";
    const String CCameraUI::MOVE_BACK_BINDING = "SceneBackward";
    const String CCameraUI::MOVE_UP_BINDING = "SceneUp";
    const String CCameraUI::MOVE_DOWN_BINDING = "SceneDown";
    const String CCameraUI::ROTATE_BINDING = "SceneRotate";
    const String CCameraUI::MOVE_BINDING = "SceneMove";
    const String CCameraUI::ZOOM_BINDING = "SceneZoom";
    const String CCameraUI::HORIZONTAL_AXIS_BINDING = "SceneHorizontal";
    const String CCameraUI::VERTICAL_AXIS_BINDING = "SceneVertical";
    const String CCameraUI::SCROLL_AXIS_BINDING = "SceneScroll";

    const float CCameraUI::START_SPEED = 4.0f;
    const float CCameraUI::TOP_SPEED = 12.0f;
    const float CCameraUI::ACCELERATION = 1.0f;
    const float CCameraUI::MOVE_SPEED = 64.0f;
    const float CCameraUI::ZOOM_SPEED = 32.0f;
    const float CCameraUI::SCROLL_SPEED = 3.0f;
    const float CCameraUI::ROTATIONAL_SPEED = 3.0f;
    const Degree CCameraUI::FIELD_OF_VIEW = Degree(90.0f);

    CCameraUI::CCameraUI(const HSceneObject& parent)
        : Component(parent, TID_CCameraUI)
        , _inputEnabled(false)
        , _currentSpeed(0.0f)
        , _lastHideCursorState(false)
        , _target(Vector3::ZERO)
    {
        // Set a name for the component, so we can find it later if needed
        SetName("CCameraUI");

        _camera = static_object_cast<CCamera>(_parent->GetComponent(TID_CCamera)->GetHandle());

        _moveForwardBtn = VirtualButton(MOVE_FORWARD_BINDING);
        _moveLeftBtn = VirtualButton(MOVE_LEFT_BINDING);
        _moveRightBtn = VirtualButton(MOVE_RIGHT_BINDING);
        _moveBackwardBtn = VirtualButton(MOVE_BACK_BINDING);
        _moveUpBtn = VirtualButton(MOVE_UP_BINDING);
        _moveDownBtn = VirtualButton(MOVE_DOWN_BINDING);

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

        bool goingForward = gVirtualInput().IsButtonHeld(_moveForwardBtn);
        bool goingBack = gVirtualInput().IsButtonHeld(_moveBackwardBtn);
        bool goingLeft = gVirtualInput().IsButtonHeld(_moveLeftBtn);
        bool goingRight = gVirtualInput().IsButtonHeld(_moveRightBtn);
        bool goingUp = gVirtualInput().IsButtonHeld(_moveUpBtn);
        bool goingDown = gVirtualInput().IsButtonHeld(_moveDownBtn);
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

                SO()->Move(direction * MOVE_SPEED * frameDelta);

                _target += direction * MOVE_SPEED * frameDelta;
            }
            else
            {
                float horzValue = gVirtualInput().GetAxisValue(_horizontalAxis);
                float vertValue = gVirtualInput().GetAxisValue(_verticalAxis);

                const Transform& tfrm = SO()->GetLocalTransform();

                SO()->RotateAround(_target, tfrm.GetRight(), Radian(Degree(vertValue)));
                SO()->RotateAround(_target, Vector3::UNIT_Y, Radian(Degree(horzValue)));

                Vector3 direction = Vector3::ZERO;

                /*if (goingForward) direction += tfrm.GetForward();
                if (goingBack) direction -= tfrm.GetForward();
                if (goingRight) direction += tfrm.GetRight();
                if (goingLeft) direction -= tfrm.GetRight();
                if (goingUp) direction += tfrm.GetUp();
                if (goingDown) direction -= tfrm.GetUp();

                if (direction.SquaredLength() != 0)
                {
                    direction.Normalize();

                    float multiplier = 1.0f;

                    _currentSpeed = Math::Clamp(_currentSpeed + ACCELERATION * frameDelta, START_SPEED, TOP_SPEED);
                    _currentSpeed *= multiplier;
                }
                else
                {
                    _currentSpeed = 0.0f;
                }

                const float tooSmall = 0.0001f;
                if (_currentSpeed > tooSmall)
                {
                    Vector3 velocity = direction * _currentSpeed;
                    SO()->Move(velocity * frameDelta);
                }*/
            }
        }
        else
        {
            float scrollAmount = Math::Clamp(gVirtualInput().GetAxisValue(_scrollAxis), -32.0f, 32.0f);

            if (fabs(scrollAmount) > 1.0f)
                scrolling(Math::Clamp(gVirtualInput().GetAxisValue(_scrollAxis), -32.0f, 32.0f), SCROLL_SPEED);
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

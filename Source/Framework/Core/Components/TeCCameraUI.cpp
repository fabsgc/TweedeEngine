#include "TeCCameraUI.h"

#include "Math/TeMath.h"
#include "Math/TeRadian.h"
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

    const float CCameraUI::MOVE_SPEED = 0.2f;
    const float CCameraUI::ZOOM_SPEED = 6.0f;
    const float CCameraUI::SCROLL_SPEED = 12.0f;
    const float CCameraUI::ROTATIONAL_SPEED = 8.0f;
    const float CCameraUI::TOP_SCROLL_SPEED = 12.0f;
    const float CCameraUI::MAX_RIGHT_ANGLE = 89.0f;
    const float CCameraUI::MAX_ZOOM_SPEED = 250.0f;
    const float CCameraUI::MAX_ZOOM = 50000.0f;
    const float CCameraUI::MIN_ZOOM = 0.5f;
    const float CCameraUI::MAX_ROTATION = 15.0f;

    CCameraUI::CCameraUI(const HSceneObject& parent)
        : Component(parent, TID_CCameraUI)
        , _cameraInitialized(false)
        , _target(Vector3::ZERO)
        , _localRotation(Vector3::ZERO)
        , _distanceToTarget(0.0f)
        , _inputEnabled(false)
        , _zoomingEnabled(false)
        , _lastHideCursorState(false)
        , _hasChanged(false)
    {
        SetName("CCameraUI");
        SetFlag(Component::AlwaysRun, true);

        _rotateBtn = VirtualButton(ROTATE_BINDING);
        _moveBtn = VirtualButton(MOVE_BINDING);
        _zoomBtn = VirtualButton(ZOOM_BINDING);

        _horizontalAxis = VirtualAxis(HORIZONTAL_AXIS_BINDING);
        _verticalAxis = VirtualAxis(VERTICAL_AXIS_BINDING);
        _scrollAxis = VirtualAxis(SCROLL_AXIS_BINDING);
    }

    void CCameraUI::InitDistanceToTarget()
    {
        _distanceToTarget = SO()->GetLocalTransform().GetPosition().Distance(_target);
    }

    void CCameraUI::InitLocalRotation()
    {
        Vector3 forwardAngle = SO()->GetLocalTransform().GetForward();
        float oppositeAngle = (-forwardAngle).AngleBetween(Vector3::UNIT_Y).ValueDegrees();
        _localRotation.x = -(180.0f - 90.0f - oppositeAngle);
    }

    void CCameraUI::Update()
    {
        _hasChanged = false;

        if (!_cameraInitialized)
        {
            if(_parent->HasComponent(TID_CCamera))
                _camera = static_object_cast<CCamera>(_parent->GetComponent(TID_CCamera)->GetHandle());

            if (!_camera)
                return;

            _cameraInitialized = true;
        }

        bool isOrtographic = _camera->GetProjectionType() == ProjectionType::PT_ORTHOGRAPHIC;
        bool camRotating = gVirtualInput().IsButtonHeld(_rotateBtn);
        bool camMoving = gVirtualInput().IsButtonHeld(_moveBtn);
        bool camZooming = gVirtualInput().IsButtonHeld(_zoomBtn) && _zoomingEnabled;
        bool hideCursor = camRotating;
        bool needsRedraw = false;
        float frameDelta = gTime().GetFrameDelta();

        if (!_inputEnabled)
        {
            if(_lastHideCursorState == hideCursor)
                Platform::ShowCursor();
            return;
        }

        if (hideCursor != _lastHideCursorState)
        {
            if (hideCursor)
            {
                Platform::HideCursor();
            }
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

        auto scrolling = [&](const float& amount, const float& sensitivity) {
            // We are too close and want to move forward again
            if (_distanceToTarget < 0.5f && amount > 0.0f)
                return;

            // We are to far and want to move backward again
            if (_distanceToTarget > MAX_ZOOM && amount < 0.0f)
                return;

            float coefficient = (_distanceToTarget < MAX_ZOOM_SPEED) ? fabs(_distanceToTarget) / 2.0f : MAX_ZOOM_SPEED;
            float scrollAmount = amount * coefficient * sensitivity * frameDelta;

            // We try to move with an amount greater that the target distance
            if (amount > 0.0f && scrollAmount > fabs(_distanceToTarget))
                scrollAmount = _distanceToTarget - 1.0f;

            if (!isOrtographic)
            {
                Vector3 forward = SO()->GetLocalTransform().GetForward();
                forward.Normalize();
                SO()->Move(forward * scrollAmount);
            }
            else
            {
                float orthoHeight = Math::Max(1.0f, _camera->GetOrthoWindowHeight() - scrollAmount);
                _camera->SetOrthoWindowHeight(orthoHeight);
            }

            InitDistanceToTarget();
            needsRedraw = true;
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

                needsRedraw = true;
            }
            else
            {
                const Transform& tfrm = SO()->GetTransform();

                float horzValue = gVirtualInput().GetAxisValue(_horizontalAxis);
                float vertValue  = gVirtualInput().GetAxisValue(_verticalAxis);

                float rotationRight = Math::Clamp(vertValue * ROTATIONAL_SPEED, -MAX_ROTATION, MAX_ROTATION);
                float rotationY = Math::Clamp(horzValue * ROTATIONAL_SPEED, -MAX_ROTATION, MAX_ROTATION);

                _localRotation.y += rotationY;

                if (fabs(_localRotation.x + rotationRight) > MAX_RIGHT_ANGLE)
                {
                    if (rotationRight > 0.0f)
                        rotationRight = MAX_RIGHT_ANGLE - _localRotation.x;
                    else
                        rotationRight = -MAX_RIGHT_ANGLE - _localRotation.x;

                    _localRotation.x += rotationRight;
                }
                else
                    _localRotation.x += rotationRight;

                if(rotationY != 0.0f)
                    SO()->RotateAround(_target, Vector3::UNIT_Y, Radian::FromDegrees(rotationY));
                if(rotationRight != 0.0f)
                    SO()->RotateAround(_target, tfrm.GetRight(), Radian::FromDegrees(rotationRight));

                needsRedraw = true;
            }
        }
        else if(_zoomingEnabled)
        {
            float scrollAmount = Math::Clamp(gVirtualInput().GetAxisValue(_scrollAxis), -TOP_SCROLL_SPEED, TOP_SCROLL_SPEED);

            if (fabs(scrollAmount) > 1.0f)
                scrolling(scrollAmount, SCROLL_SPEED);
        }

        if (needsRedraw)
        {
            _hasChanged = true;
            _camera->NotifyNeedsRedraw();
        }
    }

    void CCameraUI::EnableInput(bool enable)
    {
        _inputEnabled = enable;

        if (_lastHideCursorState)
        {
            UINT32 width = gCoreApplication().GetWindow()->GetProperties().Width;
            UINT32 height = gCoreApplication().GetWindow()->GetProperties().Height;
            UINT32 left = gCoreApplication().GetWindow()->GetProperties().Left;
            UINT32 top = gCoreApplication().GetWindow()->GetProperties().Top;

            Platform::ShowCursor();
            Platform::SetCursorPosition(Vector2I(width / 2 + left, height / 2 + top));

            _lastHideCursorState = false;
        }   
    }

    void CCameraUI::EnableZooming(bool enable)
    {
        _zoomingEnabled = enable;
    }

    void CCameraUI::SetTarget(const Vector3& target)
    {
        _target = target;
        _parent->LookAt(target);

        InitDistanceToTarget();
        InitLocalRotation();
    }

    void CCameraUI::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CCameraUI>(c));
    }

    void CCameraUI::Clone(const HCameraUI& c)
    {
        Component::Clone(c.GetInternalPtr());

        // TODO : Be careful here (I'm not sure it's a good idea to copy this component)

        //_cameraInitialized = true;

        //_target = c->_target;
        //_camera = c->_camera;

        _rotateBtn = c->_rotateBtn;
        _moveBtn = c->_moveBtn;
        _zoomBtn = c->_zoomBtn;

        _horizontalAxis = c->_horizontalAxis;
        _verticalAxis = c->_verticalAxis;
        _scrollAxis = c->_scrollAxis;
    }
}

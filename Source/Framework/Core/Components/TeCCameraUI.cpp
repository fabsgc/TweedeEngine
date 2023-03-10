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
    const float CCameraUI::ZOOM_SPEED = 12.0f;
    const float CCameraUI::SCROLL_SPEED = 12.0f;
    const float CCameraUI::ROTATIONAL_SPEED = 8.0f;
    const float CCameraUI::TOP_SCROLL_SPEED = 12.0f;
    const float CCameraUI::MAX_RIGHT_ANGLE = 89.0f;
    const float CCameraUI::MAX_ZOOM_SPEED = 250.0f;
    const float CCameraUI::MAX_ZOOM = 50000.0f;
    const float CCameraUI::MIN_ZOOM = 0.5f;
    const float CCameraUI::MAX_ROTATION = 15.0f;

    CCameraUI::CCameraUI()
        : Component(HSceneObject(), TID_CCameraUI)
        , _cameraInitialized(false)
        , _needsRedraw(false)
        , _target(Vector3::ZERO)
        , _localRotation(Vector3::ZERO)
        , _distanceToTarget(0.0f)
        , _inputEnabled(false)
        , _zoomingEnabled(false)
        , _lastHideCursorState(false)
    {
        SetName("CCameraUI");
        SetFlag(Component::AlwaysRun, true);
    }

    CCameraUI::CCameraUI(const HSceneObject& parent)
        : Component(parent, TID_CCameraUI)
        , _cameraInitialized(false)
        , _needsRedraw(false)
        , _target(Vector3::ZERO)
        , _localRotation(Vector3::ZERO)
        , _distanceToTarget(0.0f)
        , _inputEnabled(false)
        , _zoomingEnabled(false)
        , _lastHideCursorState(false)
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
        _needsRedraw = false;

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

        // fill amountLeft left if smoothMovement is true
        auto scrolling = [&](const float& iAmount, bool iSmoothMovement, float& oAmountLeft) {
            // We are too close and want to move forward again
            if (_distanceToTarget < 0.5f && iAmount > 0.0f)
            {
                oAmountLeft = 0.0f;
                return;
            }

            // We are to far and want to move backward again
            if (_distanceToTarget > MAX_ZOOM && iAmount < 0.0f)
            {
                oAmountLeft = 0.0f;
                return;
            }

            if (frameDelta > 0.004f)
                frameDelta = 0.004f;

            float coefficient = (_distanceToTarget < MAX_ZOOM_SPEED) ? fabs(_distanceToTarget) / 2.0f : MAX_ZOOM_SPEED;
            float scrollAmount = iAmount * coefficient * frameDelta;

            // We try to move with an amount greater that the target distance
            if (iAmount > 0.0f && scrollAmount > fabs(_distanceToTarget))
                scrollAmount = _distanceToTarget - 1.0f;

            float scrollAmountToDo = scrollAmount;

            if (iSmoothMovement)
            {
                if (Math::Abs(scrollAmount) < 0.02f)
                {
                    scrollAmountToDo = scrollAmount;
                    oAmountLeft = 0.0f;
                }
                else
                {
                    scrollAmountToDo = scrollAmount / 15.0f;
                    oAmountLeft = (scrollAmount - scrollAmountToDo) / coefficient / frameDelta;
                }
            }

            if (!isOrtographic)
            {
                Vector3 forward = SO()->GetLocalTransform().GetForward();
                forward.Normalize();
                SO()->Move(forward * scrollAmountToDo);
            }
            else
            {
                float orthoHeight = Math::Max(1.0f, _camera->GetOrthoWindowHeight() - scrollAmountToDo);
                _camera->SetOrthoWindowHeight(orthoHeight);
            }

            InitDistanceToTarget();
            needsRedraw = true;
        };

        if (camRotating)
        {
            if (camZooming)
            {
                _scrollAmountLeft = 0.0f;
                scrolling(gVirtualInput().GetAxisValue(_verticalAxis) * ZOOM_SPEED, false, _scrollAmountLeft);
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

                if (Math::Abs(horzValue) > 0.0f || Math::Abs(vertValue) > 0.0f)
                {
                    float rotationRight = Math::Clamp(vertValue * ROTATIONAL_SPEED, -MAX_ROTATION, MAX_ROTATION);
                    float rotationY = Math::Clamp(horzValue * ROTATIONAL_SPEED, -MAX_ROTATION, MAX_ROTATION);

                    _localRotation.y += rotationY;

                    if (Math::Abs(_localRotation.x + rotationRight) > MAX_RIGHT_ANGLE)
                    {
                        if (rotationRight > 0.0f)
                            rotationRight = MAX_RIGHT_ANGLE - _localRotation.x;
                        else
                            rotationRight = -MAX_RIGHT_ANGLE - _localRotation.x;

                        _localRotation.x += rotationRight;
                    }
                    else
                        _localRotation.x += rotationRight;

                    if (!Math::ApproxEquals(rotationY, 0.0f))
                        SO()->RotateAround(_target, Vector3::UNIT_Y, Radian::FromDegrees(rotationY));
                    if (!Math::ApproxEquals(rotationRight, 0.0f))
                        SO()->RotateAround(_target, tfrm.GetRight(), Radian::FromDegrees(rotationRight));

                    needsRedraw = true;
                }                
            }
        }
        else if(_zoomingEnabled)
        {
            float scrollAmount = Math::Clamp(gVirtualInput().GetAxisValue(_scrollAxis), -TOP_SCROLL_SPEED, TOP_SCROLL_SPEED);

            if (Math::Abs(scrollAmount) > 1.0f)
                scrolling(scrollAmount * SCROLL_SPEED + _scrollAmountLeft, true, _scrollAmountLeft);
            else if (_scrollAmountLeft != 0.0f)
                scrolling(_scrollAmountLeft, true, _scrollAmountLeft);
        }

        _needsRedraw = needsRedraw;
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

    bool CCameraUI::Clone(const HComponent& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        return Clone(static_object_cast<CCameraUI>(c), suffix);
    }

    bool CCameraUI::Clone(const HCameraUI& c, const String& suffix)
    {
        if (c.Empty())
        {
            TE_DEBUG("Tries to clone a component using an invalid component handle");
            return false;
        }

        if (Component::Clone(c.GetInternalPtr(), suffix))
        {
            _target = c->_target;
            _localRotation = c->_localRotation;
            _lastHideCursorState = c->_lastHideCursorState;

            return true;
        }

        return false;
    }
}

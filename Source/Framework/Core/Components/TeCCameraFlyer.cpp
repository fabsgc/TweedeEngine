#include "TeCCameraFlyer.h"
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
    const float CCameraFlyer::START_SPEED = 25.0f;
    const float CCameraFlyer::TOP_SPEED = 100.0f;
    const float CCameraFlyer::ACCELERATION = 8.0f;
    const float CCameraFlyer::FAST_MODE_MULTIPLIER = 2.0f;
    const float CCameraFlyer::ROTATION_SPEED = 2.0f;

    /** Wraps an angle so it always stays in [0, 360) range. */
    Degree wrapAngle(Degree angle)
    {
        if (angle.ValueDegrees() < -360.0f)
            angle += Degree(360.0f);

        if (angle.ValueDegrees() > 360.0f)
            angle -= Degree(360.0f);

        return angle;
    }

    CCameraFlyer::CCameraFlyer(const HSceneObject& parent)
        : Component(parent, TID_CCameraFlyer)
    {
        // Set a name for the component, so we can find it later if needed
        SetName("CCameraFlyer");

        // Get handles for key bindings. Actual keys attached to these bindings will be registered during app start-up.
        _moveForward = VirtualButton("Forward");
        _moveBack = VirtualButton("Back");
        _moveLeft = VirtualButton("Left");
        _moveRight = VirtualButton("Right");
        _fastMove = VirtualButton("FastMove");
        _rotateCam = VirtualButton("RotateCam");
        _horizontalAxis = VirtualAxis("Horizontal");
        _verticalAxis = VirtualAxis("Vertical");
    }

    void CCameraFlyer::Update()
    {
        // Check if any movement or rotation keys are being held
        bool goingForward = gVirtualInput().IsButtonHeld(_moveForward);
        bool goingBack = gVirtualInput().IsButtonHeld(_moveBack);
        bool goingLeft = gVirtualInput().IsButtonHeld(_moveLeft);
        bool goingRight = gVirtualInput().IsButtonHeld(_moveRight);
        bool fastMove = gVirtualInput().IsButtonHeld(_fastMove);
        bool camRotating = gVirtualInput().IsButtonHeld(_rotateCam);

        // If switch to or from rotation mode, hide or show the cursor
        if (camRotating != _lastButtonState)
        {
            if (camRotating)
                Platform::HideCursor();
            else
            {
                UINT32 width = gCoreApplication().GetWindow()->GetProperties().Width;
                UINT32 height = gCoreApplication().GetWindow()->GetProperties().Height;
                UINT32 left = gCoreApplication().GetWindow()->GetProperties().Left;
                UINT32 top = gCoreApplication().GetWindow()->GetProperties().Top;

                Platform::ShowCursor();
                Platform::SetCursorPosition(Vector2I(width/2 + left, height/2 + top));
            }

            _lastButtonState = camRotating;
        }

        // If camera is rotating, apply new pitch/yaw rotation values depending on the amount of rotation from the
        // vertical/horizontal axes.
        float frameDelta = gTime().GetFrameDelta();
        if (camRotating)
        {
            _yaw += Degree(gVirtualInput().GetAxisValue(_horizontalAxis) * ROTATION_SPEED);
            _pitch += Degree(gVirtualInput().GetAxisValue(_verticalAxis) * ROTATION_SPEED);

            _yaw = wrapAngle(_yaw);
            _pitch = wrapAngle(_pitch);

            Quaternion yRot;
            yRot.FromAxisAngle(Vector3::UNIT_Y, Radian(_yaw));

            Quaternion xRot;
            xRot.FromAxisAngle(Vector3::UNIT_X, Radian(_pitch));

            Quaternion ca_rot = yRot * xRot;
            ca_rot.Normalize();

            SO()->SetRotation(ca_rot);
        }

        const Transform& tfrm = SO()->GetTransform();

        // If the movement button is pressed, determine direction to move in
        Vector3 direction = Vector3::ZERO;
        if (goingForward) direction += tfrm.GetForward();
        if (goingBack) direction -= tfrm.GetForward();
        if (goingRight) direction += tfrm.GetRight();
        if (goingLeft) direction -= tfrm.GetRight();

        // If a direction is chosen, normalize it to determine final direction.
        if (direction.SquaredLength() != 0)
        {
            direction.Normalize();

            // Apply fast move multiplier if the fast move button is held.
            float multiplier = 1.0f;
            if (fastMove)
                multiplier = FAST_MODE_MULTIPLIER;

            // Calculate current speed of the camera
            _currentSpeed = Math::Clamp(_currentSpeed + ACCELERATION * frameDelta, START_SPEED, TOP_SPEED);
            _currentSpeed *= multiplier;
        }
        else
        {
            _currentSpeed = 0.0f;
        }

        // If the current speed isn't too small, move the camera in the wanted direction
        float tooSmall = std::numeric_limits<float>::epsilon();
        if (_currentSpeed > tooSmall)
        {
            Vector3 velocity = direction * _currentSpeed;
            SO()->Move(velocity * frameDelta);
        }
    }
}

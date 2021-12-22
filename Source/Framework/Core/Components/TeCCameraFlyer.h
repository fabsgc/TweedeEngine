#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"
#include "Math/TeMath.h"
#include "Input/TeVirtualInput.h"

namespace te
{
    /** Component that controls movement and rotation of the scene object it's attached to. */
    class TE_CORE_EXPORT CCameraFlyer : public Component
    {
    public:
        /** Return Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CCameraFlyer; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HCameraFlyer& c, const String& suffix = "");

        /** Triggered once per frame. Allows the component to handle input and move. */
        void Update() override;

        /** Returns current pitch angle (for editor) */
        const Degree& GetPitch() const { return _pitch; }

        /** Returns current yaw angle (for editor) */
        const Degree& GetYaw() const { return _yaw; }

    protected:
        friend class SceneObject;

        CCameraFlyer();
        CCameraFlyer(const HSceneObject& parent);

    private:
        float _currentSpeed = 0.0f; /**< Current speed of the camera. */

        Degree _pitch = Degree(0.0f); /**< Current pitch rotation of the camera (looking up or down). */
        Degree _yaw = Degree(0.0f); /**< Current yaw rotation of the camera (looking left or right). */
        bool _lastButtonState = false; /**< Determines was the user rotating the camera last frame. */

        VirtualButton _moveForward; /**< Key binding for moving the camera forward. */
        VirtualButton _moveBackward; /**< Key binding for moving the camera backward. */
        VirtualButton _moveLeft; /**< Key binding for moving the camera left. */
        VirtualButton _moveRight; /**< Key binding for moving the camera right. */
        VirtualButton _fastMove; /**< Key that speeds up movement while held. */
        VirtualButton _rotateCam; /**< Key that allows camera to be rotated while held. */
        VirtualAxis _verticalAxis; /**< Input device axis used for controlling camera's pitch rotation (up/down). */
        VirtualAxis _horizontalAxis; /**< Input device axis used for controlling camera's yaw rotation (left/right). */

        static const float START_SPEED; /**< Initial movement speed. */
        static const float TOP_SPEED; /**< Maximum movement speed. */
        static const float ACCELERATION; /**< Acceleration that determines how quickly to go from starting to top speed. */
        static const float FAST_MODE_MULTIPLIER; /**< Multiplier applied to the speed when the fast move button is held. */
        static const float ROTATION_SPEED; /**< Determines speed of camera rotation. */
    };
}

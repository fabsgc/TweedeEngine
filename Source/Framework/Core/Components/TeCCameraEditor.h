#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"
#include "Math/TeMath.h"
#include "Input/TeVirtualInput.h"

namespace te
{
    /** Component that controls movement and rotation of the scene object it's attached to. */
    class TE_CORE_EXPORT CCameraEditor : public Component
    {
    public:
        CCameraEditor(const HSceneObject& parent);

        /** Triggered once per frame. Allows the component to handle input and move. */
        void Update() override;

    private:
        Degree _pitch = Degree(0.0f); /**< Current pitch rotation of the camera (looking up or down). */
        Degree _yaw = Degree(0.0f); /**< Current yaw rotation of the camera (looking left or right). */
        bool _lastButtonState = false; /**< Determines was the user rotating the camera last frame. */

        VirtualButton _zoomCam; /**< Key that allows camera to be zoomed while held. */
        VirtualButton _moveCam; /**< Key that allows camera to be moved while held. */
        VirtualButton _rotateCam; /**< Key that allows camera to be rotated while held. */
        VirtualAxis _verticalAxis; /**< Input device axis used for controlling camera's pitch rotation (up/down). */
        VirtualAxis _horizontalAxis; /**< Input device axis used for controlling camera's yaw rotation (left/right). */

        static const float MOVE_SPEED; /**< Movement speed. */
        static const float ROTATION_SPEED; /**< Rotation speed. */
    };
}

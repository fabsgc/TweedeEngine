#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"
#include "Math/TeMath.h"
#include "Input/TeVirtualInput.h"

namespace te
{
    /** Component that controls movement and rotation of the scene object it's attached to. */
    class TE_CORE_EXPORT CCameraUI : public Component
    {
    public:
        CCameraUI(const HSceneObject& parent);

        /** Triggered once per frame. Allows the component to handle input and move. */
        void Update() override;

        /** Enables or disables camera controls. */
        void EnableInput(bool enable);

        /** For rotation, we want to know the current reference position. */
        void SetTarget(Vector3 target);

    public:
        static const String MOVE_FORWARD_BINDING;
        static const String MOVE_LEFT_BINDING;
        static const String MOVE_RIGHT_BINDING;
        static const String MOVE_BACK_BINDING;
        static const String MOVE_UP_BINDING;
        static const String MOVE_DOWN_BINDING;

        static const String ROTATE_BINDING;
        static const String MOVE_BINDING;
        static const String ZOOM_BINDING;

        static const String HORIZONTAL_AXIS_BINDING;
        static const String VERTICAL_AXIS_BINDING;
        static const String SCROLL_AXIS_BINDING;

        static const float START_SPEED;
        static const float TOP_SPEED;
        static const float ACCELERATION;
        static const float ZOOM_SPEED;
        static const float MOVE_SPEED;
        static const float SCROLL_SPEED;
        static const float ROTATIONAL_SPEED;
        static const Degree FIELD_OF_VIEW;

    protected:
        VirtualButton _moveForwardBtn;
        VirtualButton _moveLeftBtn;
        VirtualButton _moveRightBtn;
        VirtualButton _moveBackwardBtn;
        VirtualButton _moveUpBtn;
        VirtualButton _moveDownBtn;

        VirtualButton _rotateBtn;
        VirtualButton _moveBtn;
        VirtualButton _zoomBtn;

        VirtualAxis _horizontalAxis;
        VirtualAxis _verticalAxis;
        VirtualAxis _scrollAxis;

        bool _inputEnabled;
        float _currentSpeed;
        Degree _yaw;
        Degree _pitch;
        bool _lastHideCursorState;
        HCamera _camera;
        Vector3 _target;
    };
}

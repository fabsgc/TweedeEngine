#pragma once

#include "TeCorePrerequisites.h"
#include "Scene/TeComponent.h"
#include "Input/TeVirtualInput.h"
#include "Math/TeVector3.h"
#include "Components/TeCCamera.h"

namespace te
{
    /** Component that controls movement and rotation of the scene object it's attached to. */
    class TE_CORE_EXPORT CCameraUI : public Component
    {
    public:
        /** Returns Component type */
        static UINT32 GetComponentType() { return TypeID_Core::TID_CCameraUI; }

        /** @copydoc Component::Clone */
        bool Clone(const HComponent& c, const String& suffix = "") override;

        /** @copydoc Component::Clone */
        bool Clone(const HCameraUI& c, const String& suffix = "");

        /** Triggered once per frame. Allows the component to handle input and move. */
        void Update() override;

        /** Enables or disables camera controls. */
        void EnableInput(bool enable);

        /** Enables or disables camera zooming control */
        void EnableZooming(bool enable);

        /** For rotation, we want to know the current reference position. */
        void SetTarget(const Vector3& target);

        /** Returns current target vector (for editor) */
        const Vector3& GetTarget() const { return _target; }

        /** If linked camera has been modified during frame, return true */
        bool NeedsRedraw() const { return _needsRedraw; }

    public:
        static const String ROTATE_BINDING;
        static const String MOVE_BINDING;
        static const String ZOOM_BINDING;
        static const String HORIZONTAL_AXIS_BINDING;
        static const String VERTICAL_AXIS_BINDING;
        static const String SCROLL_AXIS_BINDING;

        static const float ZOOM_SPEED;
        static const float MOVE_SPEED;
        static const float SCROLL_SPEED;
        static const float TOP_SCROLL_SPEED;
        static const float ROTATIONAL_SPEED;
        static const float MAX_RIGHT_ANGLE;
        static const float MAX_ZOOM_SPEED;
        static const float MAX_ZOOM;
        static const float MIN_ZOOM;
        static const float MAX_ROTATION;

    protected:
        friend class SceneObject;

        CCameraUI();
        CCameraUI(const HSceneObject& parent);

        void InitDistanceToTarget();

        void InitLocalRotation();

        /** @copydoc Component::OnInitialized */
        void OnInitialized() override;

    protected:
        bool _cameraInitialized; // On first update, we get CCamera component from parent sceneObject
        bool _needsRedraw; // If something has been modified, we need to set that to true
        Vector3 _target;
        Vector3 _localRotation;
        float _distanceToTarget;
        bool _inputEnabled;
        bool _zoomingEnabled;
        bool _lastHideCursorState;

        float _scrollAmountLeft = 0.0f; // We want a smooth zoom movement.

        HCamera _camera;

        VirtualButton _rotateBtn;
        VirtualButton _moveBtn;
        VirtualButton _zoomBtn;

        VirtualAxis _horizontalAxis;
        VirtualAxis _verticalAxis;
        VirtualAxis _scrollAxis;
    };
}

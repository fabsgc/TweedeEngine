#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeMatrix4.h"
#include "Math/TeVector3.h"
#include "Math/TeQuaternion.h"
#include "Scene/TeGameObjectManager.h"
#include "Scene/TeGameObject.h"
#include "Scene/TeComponent.h"
#include "Scene/TeTransform.h"

namespace te
{
    /**
     * An object in the scene graph. It has a transform object that allows it to be positioned, scaled and rotated. It can
     * have other scene objects as children, and will have a scene object as a parent, in which case transform changes
     * to the parent are reflected to the child scene objects (children are relative to the parent).
     *
     * Each scene object can have one or multiple Component%s attached to it, where the components inherit the scene
     * object's transform, and receive updates about transform and hierarchy changes.
     */
    class TE_CORE_EXPORT SceneObject : public GameObject
    {
    public:
        /** Gets the transform object representing object's position/rotation/scale in world space. */
        const Transform& GetTransform() const;

    public:
        /**
         * Removes the component from this object, and deallocates it.
         *
         * @param[in]	component	The component to destroy.
         * @param[in]	immediate	If true, the component will be deallocated and become unusable right away. Otherwise
         *							the deallocation will be delayed to the end of frame (preferred method).
         */
        void DestroyComponent(const HComponent component, bool immediate = false);

        /**
         * Removes the component from this object, and deallocates it.
         *
         * @param[in]	component	The component to destroy.
         * @param[in]	immediate	If true, the component will be deallocated and become unusable right away. Otherwise
         *							the deallocation will be delayed to the end of frame (preferred method).
         */
        void DestroyComponent(Component* component, bool immediate = false);

    private:
        Transform _localTfrm;
        mutable Transform _worldTfrm;
    };
}
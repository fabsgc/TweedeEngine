#include "Components/TeCJoint.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

namespace te
{
    CJoint::CJoint(JOINT_DESC& desc)
        : Component(HSceneObject(), (UINT32)TID_CJoint)
        , _desc(desc)
        , _internal(nullptr)
    {
        SetName("Joint");
        SetFlag(Component::AlwaysRun, true);
    }

    CJoint::CJoint(const HSceneObject& parent, JOINT_DESC desc)
        : Component(parent, (UINT32)TID_CJoint)
        , _desc(desc)
        , _internal(nullptr)
    {
        SetName("Joint");
        SetFlag(Component::AlwaysRun, true);
    }

    CJoint::~CJoint()
    { }

    void CJoint::Initialize()
    { }

    void CJoint::Clone(const HComponent& c)
    { }

    void CJoint::Clone(const HJoint& c)
    { }

    void CJoint::OnInitialized()
    { }

    void CJoint::OnDestroyed()
    {
        if (_internal != nullptr)
            DestroyInternal();
    }

    void CJoint::OnDisabled()
    { 
        if (_internal != nullptr)
            DestroyInternal();
    }

    void CJoint::OnEnabled()
    { 
        // TODO
        RestoreInternal();
    }

    void CJoint::OnTransformChanged(TransformChangedFlags flags)
    {
        if (_internal == nullptr)
            return;

        // We're ignoring this during physics update because it would cause problems if the joint itself was moved by physics
        // Note: This isn't particularily correct because if the joint is being moved by physics but the rigidbodies
        // themselves are not parented to the joint, the transform will need updating. However I'm leaving it up to the
        // user to ensure rigidbodies are always parented to the joint in such a case (It's an unlikely situation that
        // I can't think of an use for - joint transform will almost always be set as an initialization step and not a
        // physics response).
        if (gPhysics().IsUpdateInProgress())
            return;
    }

    void CJoint::RestoreInternal()
    { }

    void CJoint::DestroyInternal()
    { }
}

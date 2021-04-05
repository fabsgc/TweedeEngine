#include "Components/TeCJoint.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

namespace te
{
    CJoint::CJoint(JOINT_DESC& desc, UINT32 type)
        : Component(HSceneObject(), type)
        , _desc(desc)
        , _internal(nullptr)
    {
        SetName("Joint");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CJoint::CJoint(const HSceneObject& parent, JOINT_DESC desc, UINT32 type)
        : Component(parent, type)
        , _desc(desc)
        , _internal(nullptr)
    {
        SetName("Joint");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CJoint::~CJoint()
    { }

    void CJoint::Initialize()
    { 
        RestoreInternal();
        Component::Initialize();
    }

    void CJoint::Clone(const HComponent& c)
    { 
        Clone(static_object_cast<CJoint>(c));
    }

    void CJoint::Clone(const HJoint& c)
    { 
        Component::Clone(c.GetInternalPtr());
    }

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
        RestoreInternal(); // TODO
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

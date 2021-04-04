#include "Components/TeCCollider.h"
#include "Scene/TeSceneObject.h"
#include "Physics/TePhysics.h"

namespace te
{
    CCollider::CCollider(UINT32 type)
        : Component(HSceneObject(), type)
        , _internal(nullptr)
    {
        SetName("Collider");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CCollider::CCollider(const HSceneObject& parent, UINT32 type)
        : Component(parent, type)
        , _internal(nullptr)
    {
        SetName("Collider");
        _notifyFlags = (TransformChangedFlags)(TCF_Parent | TCF_Transform);
    }

    CCollider::~CCollider()
    { }

    void CCollider::Initialize()
    { }

    void CCollider::Clone(const HComponent& c)
    {
        Clone(static_object_cast<CCollider>(c));
    }

    void CCollider::Clone(const HCollider& c)
    {
        Component::Clone(c.GetInternalPtr());
    }

    void CCollider::OnInitialized()
    { }

    void CCollider::OnDestroyed()
    {
        if (_internal != nullptr)
            DestroyInternal();
    }

    void CCollider::OnDisabled()
    {
        if (_internal != nullptr)
            DestroyInternal();
    }

    void CCollider::OnEnabled()
    {
        RestoreInternal(); // TODO
    }

    void CCollider::OnTransformChanged(TransformChangedFlags flags)
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

    void CCollider::RestoreInternal()
    { }

    void CCollider::DestroyInternal()
    { }
}

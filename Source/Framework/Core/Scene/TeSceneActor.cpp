#include "TeSceneActor.h"
#include "Scene/TeSceneObject.h"

namespace te
{
    void SceneActor::SetTransform(const Transform& transform)
    {
        if (_mobility != ObjectMobility::Movable)
        {
            return;
        }

        _transform = transform;
    }

    void SceneActor::SetMobility(ObjectMobility mobility)
    {
        _mobility = mobility;
    }

    void SceneActor::SetActive(bool active)
    {
        _active = active;
    }

    void SceneActor::_updateState(const SceneObject& so, bool force)
    {
        UINT32 curHash = so.GetTransformHash();
        if (curHash != _hash || force)
        {
            SetTransform(so.GetTransform());
            _hash = curHash;
        }

        if (so.GetActive() != _active || force)
            SetActive(so.GetActive());

        if (so.GetMobility() != _mobility || force)
            SetMobility(so.GetMobility());
    }
}

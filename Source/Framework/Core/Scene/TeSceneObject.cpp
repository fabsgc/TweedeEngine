#include "Scene/TeSceneObject.h"
#include "Scene/TeComponent.h"
#include "Scene/TeSceneManager.h"
#include "Scene/TeGameObjectManager.h"
#include "Math/TeMatrix3.h"
#include "TeCoreApplication.h"

namespace te
{
    const Transform& SceneObject::GetTransform() const
    {
        //if (!isCachedWorldTfrmUpToDate()) TODO
        //    updateWorldTfrm();

        return _worldTfrm;
    }

    void SceneObject::DestroyComponent(const HComponent component, bool immediate)
    {
    }

    void SceneObject::DestroyComponent(Component* component, bool immediate)
    {
    }
}

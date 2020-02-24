#include "TeCCameraEditor.h"
#include "Scene/TeSceneObject.h"

namespace te
{
    CCameraEditor::CCameraEditor(const HSceneObject& parent)
        : Component(parent, TID_CCameraFlyer)
    { }

    void CCameraEditor::Update()
    { }
}

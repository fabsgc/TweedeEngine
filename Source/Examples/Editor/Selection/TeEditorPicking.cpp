#include "TeEditorPicking.h"

#include "Components/TeCRenderable.h"
#include "Scene/TeSceneObject.h"
#include "TeHudPickingMat.h"
#include "Utility/TeTime.h"
#include "Mesh/TeMesh.h"

namespace te
{
    EditorPicking::EditorPicking()
        : Picking()
        , _hudMaterial(nullptr)
    { }

    void EditorPicking::Initialize()
    {
        _hudMaterial = HudPickingMat::Get();

        Picking::Initialize();
    }

    void EditorPicking::Draw(const HCamera& camera, const HSceneObject& sceneObject)
    {
        Vector<PickingUtils::PerHudInstanceData> instancedElements;

        DrawInternal(camera, sceneObject, instancedElements);

        if (instancedElements.size() > 0)
        {
            _hudMaterial->BindCamera(camera, PickingUtils::RenderType::Picking);

            RenderAPI& rapi = RenderAPI::Instance();
            rapi.SetVertexDeclaration(_instanceBuffer.PointVDecl);
            rapi.SetVertexBuffers(0, &_instanceBuffer.PointVB, 1);
            rapi.SetDrawOperation(DOT_POINT_LIST);

            UINT64 elementToDraw = static_cast<UINT64>(instancedElements.size());

            auto iterBegin = instancedElements.begin();
            auto iterRangeStart = iterBegin;
            auto iterRangeEnd = iterBegin + ((elementToDraw >= MAX_HUD_INSTANCED_BLOCK) ? MAX_HUD_INSTANCED_BLOCK : elementToDraw);

            do
            {
                UINT64 elementsDrawn = static_cast<UINT32>(iterRangeEnd - iterRangeStart);

                _hudMaterial->BindHud(iterRangeStart, iterRangeEnd);
                _hudMaterial->Bind();
                rapi.Draw(0, 1, static_cast<UINT32>(elementsDrawn));

                elementToDraw = elementToDraw - elementsDrawn;

                iterRangeStart = iterRangeEnd;
                iterRangeEnd = iterRangeStart + ((elementToDraw >= MAX_HUD_INSTANCED_BLOCK) ? MAX_HUD_INSTANCED_BLOCK : elementToDraw);
            } while (elementToDraw > 0);
        }
    }

    void EditorPicking::DrawInternal(const HCamera& camera, const HSceneObject& sceneObject, Vector<PickingUtils::PerHudInstanceData>& instancedElements)
    {
        float now = gTime().GetTime();

        for (const auto& component : sceneObject->GetComponents())
        {
            UINT32 type = component->GetCoreType();
            Color color = component->GetGameObjectColor();
            RGBA rgbaColor = color.GetAsRGBA();

            if (_colorToGameObject.find(rgbaColor) == _colorToGameObject.end())
                _colorToGameObject[rgbaColor] = GameObjectInfo(component.GetInternalPtr());

            _colorToGameObject[rgbaColor].LastUse = now;

            switch (type)
            {
            case TypeID_Core::TID_CRenderable:
            {
                HRenderable renderable = static_object_cast<CRenderable>(component);
                if (renderable->GetActive() && gRendererUtility().DoFrustumCulling(camera, renderable))
                    DrawRenderable(renderable);
            }
            break;

            default:
            break;
            }

            PickingUtils::FillPerInstanceHud(instancedElements, camera, component, PickingUtils::RenderType::Picking);
        }

        for (const auto& childSO : sceneObject->GetChildren())
            DrawInternal(camera, childSO, instancedElements);
    }
}

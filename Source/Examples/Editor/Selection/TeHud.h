#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeVertexBuffer.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "RenderAPI/TeVertexDeclaration.h"
#include "Math/TeVector3.h"
#include "../TeEditorUtils.h"

namespace te
{
    class HudPickingMat;

    class Hud
    {
    public:
        Hud() = default;
        ~Hud() = default;

        /** Init context, shader */
        void Initialize();

        /** Take a list of component (lights, cameras and render billboard on top of previous render) */
        void Render(const HCamera& camera, const HSceneObject& root);

        /** Generate a list of all hud elements to draw by iterating through the scene graph */
        void GetHudElements(const HCamera& camera, const HSceneObject& sceneObject, Vector<EditorUtils::PerHudInstanceData>& matElements);

    private:
        struct VertexBufferLayout
        {
            Vector3 Position;
        };

    private:
        HudPickingMat* _material;

        SPtr<VertexBuffer> _pointVB;
        SPtr<VertexDataDesc> _pointVDesc;
        SPtr<VertexDeclaration> _pointVDecl;
        VertexBufferLayout* _pointData = nullptr;
    };
}
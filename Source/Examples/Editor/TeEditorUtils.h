#pragma once

#include "TeCorePrerequisites.h"
#include "Image/TeTexture.h"
#include "Mesh/TeMesh.h"
#include "RenderAPI/TeRenderTexture.h"

namespace te
{
    class EditorUtils
    {
    public:

        struct PerHudInstanceData
        {
            Matrix4 MatWorldNoScale;
            Vector4 Color;
            float   Type;
            Vector3 Padding;
        };

        struct RenderWindowData
        {
            TEXTURE_DESC TargetColorDesc;
            TEXTURE_DESC TargetDepthDesc;
            RENDER_TEXTURE_DESC RenderTexDesc;
            HTexture ColorTex;
            HTexture DepthStencilTex;
            SPtr<RenderTexture> RenderTex;
            TextureSurface ColorTexSurface;
            UINT32 Width = 640;
            UINT32 Height = 480;
        };

    public:
        /** When we load a mesh using GUI, we can generate materials to apply on its submeshes */
        static void ImportMeshMaterials(HMesh& mesh);

        /** Generate a render texture using data given in parameters */
        static void GenerateViewportRenderTexture(RenderWindowData& renderData);

        /** Do a frustum culling on a light. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const HLight& light);

        /** Do a frustum culling on a scene camera. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const HCamera& sceneCamera);

        /** Do a more generic frustum culling */
        static bool DoFrustumCulling(const HCamera& camera, const Sphere& boundingSphere, const float& cullDistanceFactor);
    };
}

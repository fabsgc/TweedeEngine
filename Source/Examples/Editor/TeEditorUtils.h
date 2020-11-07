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

    };
}

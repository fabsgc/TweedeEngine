#pragma once

#include "TeCorePrerequisites.h"
#include "Image/TeTexture.h"
#include "Mesh/TeMesh.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Components/TeCCamera.h"
#include "Components/TeCLight.h"
#include "Components/TeCRenderable.h"
#include "Components/TeCAudioListener.h"
#include "Components/TeCAudioSource.h"

namespace te
{
    class EditorUtils
    {
    public:
        static const String DELETE_BINDING;
        static const String COPY_BINDING;
        static const String PASTE_BINDING;

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

        /** Do a frustum culling on a renderable. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const SPtr<CRenderable> renderable);

        /** Do a frustum culling on a light. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const SPtr<CLight> light);

        /** Do a frustum culling on a scene camera. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const SPtr<CCamera> sceneCamera);

        /** Do a frustum culling on an audio listener. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const SPtr<CAudioListener> light);

        /** Do a frustum culling on an audio source. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const SPtr<CAudioSource> light);

        /** Do a frustum culling on a renderable. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const HRenderable& renderable)
        {
            return DoFrustumCulling(camera, renderable.GetInternalPtr());
        }

        /** Do a frustum culling on a light. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const HLight& light)
        {
            return DoFrustumCulling(camera, light.GetInternalPtr());
        }

        /** Do a frustum culling on a scene camera. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const HCamera& sceneCamera)
        {
            return DoFrustumCulling(camera, sceneCamera.GetInternalPtr());
        }

        /** Do a frustum culling on an audio listener. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const HAudioListener& audio)
        {
            return DoFrustumCulling(camera, audio.GetInternalPtr());
        }

        /** Do a frustum culling on an audio source. Returns true if visible */
        static bool DoFrustumCulling(const HCamera& camera, const HAudioSource& audio)
        {
            return DoFrustumCulling(camera, audio.GetInternalPtr());
        }

        /** Do a more generic frustum culling */
        static bool DoFrustumCulling(const HCamera& camera, const Sphere& boundingSphere, const float& cullDistanceFactor);
    };
}

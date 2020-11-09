#pragma once

#include "TeCorePrerequisites.h"

#include "Renderer/TeCamera.h"
#include "Scene/TeSceneObject.h"
#include "Material/TeShader.h"
#include "Math/TeVector2I.h"
#include "Math/TeVector4.h"
#include "Math/TeMatrix4.h"
#include "Image/TeTexture.h"
#include "Image/TeColor.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Renderer/TeParamBlocks.h"
#include "../TeEditorUtils.h"

#include <unordered_map>

namespace te
{
    class GpuPickingMat;

    class GpuPicking
    {
    public:
        struct RenderParam
        {
            RenderParam(UINT32 width, UINT32 height)
                : Width(width)
                , Height(height)
            { }

            UINT32 Width;
            UINT32 Height;
        };
        
        struct GameObjectInfo
        {
            GameObjectInfo()
                : Object()
                , LastUse(0.0f)
            { }

            GameObjectInfo(WPtr<GameObject> object)
                : Object(object)
                , LastUse(0.0f)
            { }

            WPtr<GameObject> Object;
            float LastUse = 0.0f;
        };

    public:
        GpuPicking() = default;
        ~GpuPicking() = default;

        /** Init context, shader */
        void Initialize();

        /**
         * Generate up to date render texture with all renderables/lights/cameras. 
         * As it's a one time compute, it's not necessary to deal with huge culling optimization
         *
         * @param[in]	camera				handle to the camera used by 3D viewport
         * @param[in]	param           	Param to take into account for this render
         * @param[in]	root                root scene object of the scene
         */
        void ComputePicking(const HCamera& camera, const RenderParam& param, const HSceneObject& root);

        /** Returns pixel color at given position */
        Color GetColorAt(UINT32 x, UINT32 y);

        /** @copydoc GetColorAt */
        Color GetColorAt(const Vector2I& pixel) { return GetColorAt(pixel.x, pixel.y); }

        /** Returns game object instance id at given position */
        SPtr<GameObject> GetGameObjectAt(UINT32 x, UINT32 y);

        /** @copydoc GetGameObjectAt */
        SPtr<GameObject> GetGameObjectAt(const Vector2I& pixel) { return GetGameObjectAt(pixel.x, pixel.y); }

        /** Only used for debugging, returns last picking render texture */
        SPtr<RenderTexture>& GetLastPicking() { return _renderData.RenderTex; }

    private:
        /** It's can be necessary to update render texture to match viewport dimensions */
        bool CheckRenderTexture(UINT32 width, UINT32 height);

        /** Do a frustum culling on a renderable. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const HRenderable& renderable);

        /** Do a frustum culling on a light. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const HLight& light);

        /** Do a frustum culling on a scene camera. Returns true if visible */
        bool DoFrustumCulling(const HCamera& camera, const HCamera& sceneCamera);

        /** Recursive method to draw components under a sceneObject */
        void Draw(const HCamera& camera, const HSceneObject& sceneObject);

        /** @copydoc GpuPicking::Draw */
        void DrawInternal(const HCamera& camera, const HSceneObject& sceneObject, Vector<HLight>& lights, Vector<HCamera>& cameras);

        /** Specific way to draw a renderable */
        void DrawRenderable(const HRenderable& renderable);

        /** Specific way to draw a light */
        void DrawLights(const Vector<HLight>& light);

        /** Specific way to draw a camera */
        void DrawCameras(const Vector<HCamera>& light);

        /** After picking texture is rendered, we clean _colorToGameObject to remove unused objects */
        void CleanGameObjectsList();

    private:
        GpuPickingMat* _material;
        EditorUtils::RenderWindowData _renderData;
        std::unordered_map<RGBA, GameObjectInfo> _colorToGameObject;
    };
}

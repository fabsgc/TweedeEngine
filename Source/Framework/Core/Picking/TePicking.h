#pragma once

#include "TeCorePrerequisites.h"

#include "Image/TeColor.h"
#include "Picking/TePickingUtils.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    class PickingMat;

    /**
     * Class that controls GPU Picking of GameObjects.
     */
    class TE_CORE_EXPORT Picking
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
        Picking();
        ~Picking() = default;

        /** Init context, shader */
        virtual void Initialize();

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

    protected:
        /** Recursive method to draw components under a sceneObject */
        virtual void Draw(const HCamera& camera, const HSceneObject& sceneObject);

        /** @copydoc Picking::Draw */
        virtual void DrawInternal(const HCamera& camera, const HSceneObject& sceneObject, Vector<PickingUtils::PerHudInstanceData>& instancedElements);

        /** It's can be necessary to update render texture to match viewport dimensions */
        bool CheckRenderTexture(UINT32 width, UINT32 height);

        /** Specific way to draw a renderable */
        void DrawRenderable(const HRenderable& renderable);

        /** After picking texture is rendered, we clean _colorToGameObject to remove unused objects */
        void CleanGameObjectsList();

    protected:
        PickingMat* _material;
        RendererUtility::RenderWindowData _renderData;
        UnorderedMap<RGBA, GameObjectInfo> _colorToGameObject;
        PickingUtils::HudInstanceBuffer _instanceBuffer;
    };
}

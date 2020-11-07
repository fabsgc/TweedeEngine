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
#include "TeEditorUtils.h"

namespace te
{
    TE_PARAM_BLOCK_BEGIN(PerCameraParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatViewProj)
    TE_PARAM_BLOCK_END

    TE_PARAM_BLOCK_BEGIN(PerObjectParamDef)
        TE_PARAM_BLOCK_ENTRY(Matrix4, gMatWorld)
        TE_PARAM_BLOCK_ENTRY(Vector4, gColor)
    TE_PARAM_BLOCK_END

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

        /** Return pixel color at given position */
        Color GetColorAt(UINT32 x, UINT32 y);

        /** @copydoc GetColorAt */
        Color GetColorAt(const Vector2I& pixel) { return GetColorAt(pixel.x, pixel.y); }

        /** Only used for debugging, returns last picking render texture */
        SPtr<RenderTexture>& GetLastPicking() { return _renderData.RenderTex; }

    private:
        /** It's can be necessary to update render texture to match viewport dimensions */
        bool CheckRenderTexture(UINT32 width, UINT32 height);

    private:
        HShader _shader;
        EditorUtils::RenderWindowData _renderData;

        PerCameraParamDef _perCameraParamDef;
        PerObjectParamDef _perObjectParamDef;
        SPtr<GpuParamBlockBuffer> _perCameraParamBuffer;
        SPtr<GpuParamBlockBuffer> _perObjectParamBuffer;
    };
}

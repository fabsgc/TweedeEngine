#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Image/TeTexture.h"
#include "TeWidget.h"

namespace te
{
    class WidgetViewport : public Widget
    {
    public:
        WidgetViewport();
        ~WidgetViewport();

        virtual void Initialize() override;
        virtual void Update() override;

        void Resize();

    protected:
        void ResetViewport();

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

    protected:
        /** Return true if texture has been updated */
        bool CheckRenderTexture(const float& width, const float& height);

    protected:
#if TE_PLATFORM == TE_PLATFORM_WIN32
        HCamera _sceneCamera;
        HCameraUI _sceneCameraUI;
        HSceneObject _sceneCameraSO;
#endif

        RenderWindowData _renderData;
        float _lastRenderDataUpatedTime;
        bool _needResetViewport;

        static const float MIN_TIME_BETWEEN_UPDATE;
    };
}

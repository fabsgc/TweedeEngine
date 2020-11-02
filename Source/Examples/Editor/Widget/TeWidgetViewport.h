#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Input/TeVirtualInput.h"
#include "Image/TeTexture.h"
#include "Utility/TeEvent.h"
#include "../TeEditor.h"
#include "TeWidget.h"

namespace te
{
    class WidgetViewport : public Widget
    {
    public:
        static const String RETARGET_BINDING;

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
        WidgetViewport();
        ~WidgetViewport();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

        void Resize();
        void NeedsRedraw();
        void SetVisible(bool isVisible);

    protected:
        /** Return true if texture has been updated */
        bool CheckRenderTexture(const float& width, const float& height);
        
        /** Compute render target if necessary then display render texture inside the current editor viewport */
        void ResetViewport();

    protected:
        static const float MIN_TIME_BETWEEN_UPDATE;

    protected:
        HEvent _resizeEvent;
        Editor::SelectionData& _selections;

        RenderWindowData _renderData;
        HCamera _viewportCamera;
        HCameraUI& _viewportCameraUI;

        float _lastRenderDataUpatedTime;
        bool _needResetViewport;

        VirtualButton _reTargetBtn;
    };
}

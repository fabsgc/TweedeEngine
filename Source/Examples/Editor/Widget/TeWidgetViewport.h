#pragma once

#include "TeCorePrerequisites.h"

#include "../TeEditor.h"
#include "TeWidget.h"
#include "Image/TeTexture.h"
#include "Utility/TeEvent.h"
#include "Scene/TeTransform.h"
#include "Input/TeVirtualInput.h"
#include "RenderAPI/TeRenderTexture.h"
#include "Renderer/TeRendererUtility.h"

namespace te
{
    class WidgetViewport : public Widget
    {
    public:
        static const String RETARGET_BINDING;
        static const String PICKING_BINDING;

    public:
        WidgetViewport();
        ~WidgetViewport();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

        void Resize();
        void NeedsRedraw();
        void SetVisible(bool isVisible);

        const RendererUtility::RenderWindowData& GetRenderWindowData() const { return _renderData; }

    protected:
        /** Return true if texture has been updated */
        bool CheckRenderTexture(const float& width, const float& height);
        
        /** Compute render target if necessary then display render texture inside the current editor viewport */
        void ResetViewport();

        /** Handle for actions when hovering viewport */
        void HandleKeyShortcuts();

    protected:
        static const float MIN_TIME_BETWEEN_UPDATE;
        
    protected:
        HEvent _resizeEvent;
        Editor::SelectionData& _selections;

        RendererUtility::RenderWindowData _renderData;
        HCamera _viewportCamera;
        HCameraUI& _viewportCameraUI;

        // When we are in editor mode (no realtime rendering)
        // we try to have the minimum of renderer calls
        float _lastRenderDataUpatedTime;
        bool _needResetViewport;

        // We keep old camera transform in order to know if view has changed
        Transform _prevCameraTfrm;

        VirtualButton _reTargetBtn;
        VirtualButton _pickingBtn;
        VirtualButton _deleteBtn;
        VirtualButton _copyBtn;
        VirtualButton _pasteBtn;
    };
}

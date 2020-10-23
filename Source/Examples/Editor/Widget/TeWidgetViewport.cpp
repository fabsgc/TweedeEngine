#include "TeWidgetViewport.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "TeCoreApplication.h"
#include "Renderer/TeCamera.h"
#include "Scene/TeSceneObject.h"
#include "Components/TeCCamera.h"
#include "Components/TeCCameraUI.h"
#include "Utility/TeEvent.h"
#include "Gui/TeGuiAPI.h"
#include "Utility/TeTime.h"

namespace te
{
    const float WidgetViewport::MIN_TIME_BETWEEN_UPDATE = 1.0f;

    WidgetViewport::WidgetViewport()
        : Widget(WidgetType::Viewport)
        , _lastRenderDataUpatedTime(0.0f)
        , _needResetViewport(true)
    {
        _title = VIEWPORT_TITLE;
        _size = Vector2(640, 480);
        _flags |= ImGuiWindowFlags_NoScrollbar;
        _padding = Vector2(0.0f, 0.0f);
    }

    WidgetViewport::~WidgetViewport()
    { }

    void WidgetViewport::Initialize()
    {
        bool renderTextureUpdated = CheckRenderTexture((float)_renderData.Width, (float)_renderData.Height);

        // ######################################################
        _viewportSO = SceneObject::Create("UIViewport");

        _sceneCameraSO = SceneObject::Create("UICamera");
        _sceneCameraSO->SetParent(_viewportSO);

        _sceneCamera = _sceneCameraSO->AddComponent<CCamera>();
        _sceneCamera->GetViewport()->SetClearColorValue(Color(0.42f, 0.67f, 0.94f, 1.0f));
        _sceneCamera->GetViewport()->SetTarget(_renderData.RenderTex);
        _sceneCamera->Initialize();
        _sceneCamera->SetMSAACount(gCoreApplication().GetWindow()->GetDesc().MultisampleCount);
        _sceneCamera->SetProjectionType(ProjectionType::PT_PERSPECTIVE);

        _sceneCameraUI = _sceneCameraSO->AddComponent<CCameraUI>();

        _sceneCameraSO->SetPosition(Vector3(3.5f, 2.5f, 4.0f));
        _sceneCameraSO->LookAt(Vector3(0.0f, 0.75f, 0.0f));

        _sceneCameraUI->SetTarget(Vector3(0.0f, 0.75f, 0.0f));

        if (renderTextureUpdated)
            _sceneCamera->SetAspectRatio((float)_renderData.Width / (float)_renderData.Height);

        auto settings = _sceneCamera->GetRenderSettings();
        settings->ExposureScale = 0.85f;
        settings->Gamma = 1.0f;
        settings->Contrast = 2.0f;
        settings->Brightness = -0.1f;
        settings->MotionBlur.Enabled = false;
        // ######################################################

        _cameraSettings = _sceneCamera->GetRenderSettings();

        _onBeginCallback = [this] {
            if (ImGui::IsWindowFocused())
                _sceneCameraUI->EnableInput(true);
            else
                _sceneCameraUI->EnableInput(false);
        };

        gCoreApplication().GetWindow()->OnResized.Connect(std::bind(&WidgetViewport::Resize, this));
    }

    void WidgetViewport::Resize()
    {
        if (_isVisible && GuiAPI::Instance().IsGuiInitialized())
            _needResetViewport = true;
    }

    void WidgetViewport::NeedsRedraw()
    {
        _needResetViewport = true;
        _sceneCamera->NotifyNeedsRedraw();
    }

    void WidgetViewport::ResetCameraSettings()
    {
        _sceneCamera->SetRenderSettings(_cameraSettings);
    }

    void WidgetViewport::Update()
    {
        if (_isVisible && GuiAPI::Instance().IsGuiInitialized())
            ResetViewport();

        UINT32 flags = _sceneCamera->GetFlags();
        if (!gCoreApplication().GetState().IsFlagSet(ApplicationState::Game)) //We are in editor mode
        {
            if (!(flags & (UINT32)CameraFlag::OnDemand))
            {
                flags |= (UINT32)CameraFlag::OnDemand;
                _sceneCamera->SetFlags(flags);
            }
        }
        else
        {
            if (flags & (UINT32)CameraFlag::OnDemand)
            {
                flags &= ~(UINT32)CameraFlag::OnDemand;
                _sceneCamera->SetFlags(flags);
            }
        }
    }

    void WidgetViewport::UpdateBackground()
    {
        UINT32 flags = _sceneCamera->GetFlags();
        if (gCoreApplication().GetState().IsFlagSet(ApplicationState::Game)) //We are in simulation mode, we switch to edit mode and change camera to onDemand
        {
            gCoreApplication().GetState().SetFlag(ApplicationState::Game, false);

            if (!(flags & (UINT32)CameraFlag::OnDemand))
            {
                flags |= (UINT32)CameraFlag::OnDemand;
                _sceneCamera->SetFlags(flags);
            }
        }
    }

    void WidgetViewport::SetVisible(bool isVisible)
    {
        Widget::SetVisible(isVisible);
        NeedsRedraw();
    }

    void WidgetViewport::ResetViewport()
    {
        float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
        float height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

        if (CheckRenderTexture(width, height))
        {
            _sceneCamera->GetViewport()->SetTarget(_renderData.RenderTex);
            _sceneCamera->SetAspectRatio(width / height);
        }

        SPtr<TextureView> textureView = _renderData.RenderTex->GetColorTexture(0)->RequestView(
            _renderData.ColorTexSurface.MipLevel,
            _renderData.ColorTexSurface.NumMipLevels,
            _renderData.ColorTexSurface.Face,
            _renderData.ColorTexSurface.NumFaces,
            GVU_DEFAULT
        );

        void* rawData = textureView->GetRawData();

        ImGui::Image(
            static_cast<ImTextureID>(rawData),
            ImVec2(static_cast<float>(width), static_cast<float>(height)),
            ImVec2(0, 0),
            ImVec2(1, 1)
        );
    }

    bool WidgetViewport::CheckRenderTexture(const float& width, const float& height)
    {
        float deltaElapsedTime = gTime().GetTime() - _lastRenderDataUpatedTime;

        if ((UINT32)width == _renderData.Width && (UINT32)height == _renderData.Height)
            return false;

        if (deltaElapsedTime < MIN_TIME_BETWEEN_UPDATE && _lastRenderDataUpatedTime > 10.0f && _needResetViewport == false)
            return false;

        _renderData.Width = (UINT32)width;
        _renderData.Height = (UINT32)height;

        if (_renderData.RenderTex)
            _renderData.RenderTex = nullptr;
        if (_renderData.ColorTex.GetHandleData())
            _renderData.ColorTex.Release();
        if (_renderData.DepthStencilTex.GetHandleData())
            _renderData.DepthStencilTex.Release();

        _renderData.TargetColorDesc.Type = TEX_TYPE_2D;
        _renderData.TargetColorDesc.Width = _renderData.Width;
        _renderData.TargetColorDesc.Height = _renderData.Height;
        _renderData.TargetColorDesc.Format = PF_RGBA16F;
        _renderData.TargetColorDesc.NumSamples = gCoreApplication().GetWindow()->GetDesc().MultisampleCount;
        _renderData.TargetColorDesc.Usage = TU_RENDERTARGET;

        _renderData.TargetDepthDesc.Type = TEX_TYPE_2D;
        _renderData.TargetDepthDesc.Width = _renderData.Width;
        _renderData.TargetDepthDesc.Height = _renderData.Height;
        _renderData.TargetDepthDesc.Format = PF_RGBA8;
        _renderData.TargetDepthDesc.NumSamples = gCoreApplication().GetWindow()->GetDesc().MultisampleCount;
        _renderData.TargetDepthDesc.Usage = TU_DEPTHSTENCIL;

        _renderData.ColorTex = Texture::Create(_renderData.TargetColorDesc);
        _renderData.DepthStencilTex = Texture::Create(_renderData.TargetDepthDesc);

        _renderData.RenderTexDesc.ColorSurfaces[0].Tex = _renderData.ColorTex.GetInternalPtr();
        _renderData.RenderTexDesc.ColorSurfaces[0].Face = 0;
        _renderData.RenderTexDesc.ColorSurfaces[0].MipLevel = 0;

        _renderData.RenderTexDesc.DepthStencilSurface.Tex = _renderData.DepthStencilTex.GetInternalPtr();
        _renderData.RenderTexDesc.DepthStencilSurface.Face = 0;
        _renderData.RenderTexDesc.DepthStencilSurface.MipLevel = 0;

        _renderData.RenderTex = RenderTexture::Create(_renderData.RenderTexDesc);

        _lastRenderDataUpatedTime = gTime().GetTime();
        _needResetViewport = false;

        _sceneCamera->NotifyNeedsRedraw();

        return true;
    }
}
#include "TeWidgetViewport.h"

#include "Scene/TeSceneObject.h"
#include "TeCoreApplication.h"
#include "Components/TeCCamera.h"
#include "Components/TeCCameraUI.h"
#include "Gui/TeGuiAPI.h"
#include "Utility/TeTime.h"
#include "../TeEditorUtils.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    const float WidgetViewport::MIN_TIME_BETWEEN_UPDATE = 0.5f;
    const String WidgetViewport::RETARGET_BINDING = "ReTarget";
    const String WidgetViewport::PICKING_BINDING = "Picking";

    void UpdateCameraFlag(HCamera& camera);

    WidgetViewport::WidgetViewport()
        : Widget(WidgetType::Viewport)
        , _selections(gEditor().GetSelectionData())
        , _viewportCamera(gEditor().GetPreviewViewportCamera())
        , _viewportCameraUI(gEditor().GetViewportCameraUI())
        , _lastRenderDataUpatedTime(0.0f)
        , _needResetViewport(true)
    {
        _title = VIEWPORT_TITLE;
        _size = Vector2(640, 480);
        _flags |= ImGuiWindowFlags_NoScrollbar;
        _padding = Vector2(0.0f, 0.0f);
    }

    WidgetViewport::~WidgetViewport()
    {
        if (_renderData.ColorTex.IsLoaded())
            _renderData.ColorTex.Release();
        if (_renderData.DepthStencilTex.IsLoaded())
            _renderData.DepthStencilTex.Release();

        _resizeEvent.Disconnect();
    }

    void WidgetViewport::Initialize()
    {
        _resizeEvent = gCoreApplication().GetWindow()->OnResized.Connect(std::bind(&WidgetViewport::Resize, this));

        _reTargetBtn = VirtualButton(RETARGET_BINDING);
        _pickingBtn  = VirtualButton(PICKING_BINDING);
        _deleteBtn = VirtualButton(EditorUtils::DELETE_BINDING);
        _copyBtn = VirtualButton(EditorUtils::COPY_BINDING);
        _pasteBtn = VirtualButton(EditorUtils::PASTE_BINDING);

        bool renderTextureUpdated = CheckRenderTexture((float)_renderData.Width, (float)_renderData.Height);
        if (renderTextureUpdated)
            _viewportCamera->SetAspectRatio((float)_renderData.Width / (float)_renderData.Height);

        _viewportCamera->GetViewport()->SetTarget(_renderData.RenderTex);

        auto OnRetarget = [this] {
            if (gVirtualInput().IsButtonDown(_reTargetBtn))
            {
                if (_selections.ClickedComponent)
                {
                    _viewportCameraUI->SetTarget(_selections.ClickedComponent->GetSceneObject()->GetTransform().GetPosition());
                    gEditor().NeedsRedraw();
                }
                else if (_selections.ClickedSceneObject)
                {
                    _viewportCameraUI->SetTarget(_selections.ClickedSceneObject->GetTransform().GetPosition());
                    gEditor().NeedsRedraw();
                }
            }
        };

        _onBeginCallback = [this, OnRetarget] {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 viewportPos(mousePos.x - windowPos.x, mousePos.y - windowPos.y - 26);

            _viewportCameraUI->EnableZooming(false);

            // CCamerUI component is active only when original viewport camera is active
            if (ImGui::IsWindowFocused() && _viewportCamera.GetInternalPtr() == gEditor().GetViewportCamera().GetInternalPtr())
            {
                _viewportCameraUI->EnableInput(true);
                OnRetarget();
            }
            else if (ImGui::IsWindowHovered() && ImGui::GetIO().MouseDown[ImGuiMouseButton_Right]
                && _viewportCamera.GetInternalPtr() == gEditor().GetViewportCamera().GetInternalPtr())
            {
                _viewportCameraUI->EnableInput(true);
            }
            else
            {
                _viewportCameraUI->EnableInput(false);
            }

            // Handle zooming if mouse is hovering windows (even if windows not focused)
            if (viewportPos.x > 0.0f && viewportPos.x < _renderData.Width && viewportPos.y > 0.0f && viewportPos.y < _renderData.Height && ImGui::IsWindowHovered())
            {
                _viewportCameraUI->EnableInput(true);
                _viewportCameraUI->EnableZooming(true);
                OnRetarget();
            }
        };
    }

    void WidgetViewport::Resize()
    {
        if (_isVisible && GuiAPI::Instance().IsGuiInitialized())
        {
            _viewportCamera->NotifyNeedsRedraw();
            gEditor().MakePickingDirty();
            gEditor().MakeSelectionDirty();
            gEditor().MakeHudDirty();
            gEditor().MakePhysicsDirty();
        }
    }

    void WidgetViewport::NeedsRedraw()
    {
        // If current camera has been destroyed, we fall back to the default camera
        if (_viewportCamera.IsDestroyed())
        {
            _viewportCamera = gEditor().GetPreviewViewportCamera().GetNewHandleFromExisting();
            _viewportCamera->GetViewport()->SetTarget(_renderData.RenderTex);
        }

        // If current camera has been switched, we switch camera, set render target and update flags
        if (_viewportCamera.GetInternalPtr() != gEditor().GetPreviewViewportCamera().GetInternalPtr())
        {
            // only one camera can write to render target at a time
            _viewportCamera->GetViewport()->SetTarget(nullptr);
            _viewportCamera = gEditor().GetPreviewViewportCamera().GetNewHandleFromExisting();
            _viewportCamera->GetViewport()->SetTarget(_renderData.RenderTex);

            // When we change camera, maybe the new one has not a correct aspect ratio
            _viewportCamera->SetAspectRatio((float)_renderData.Width / (float)_renderData.Height);

            // Update all renderer data for the current camera
            UpdateCameraFlag(_viewportCamera);
        }

        // Notify camera has been updated
        if (!gCoreApplication().GetState().IsFlagSet(ApplicationState::Game))
            _viewportCamera->NotifyUpdateEverything(); // updating everything is very heavy

        _viewportCamera->NotifyNeedsRedraw();
        _needResetViewport = true;
    }

    void WidgetViewport::Update()
    {
        if (gCoreApplication().GetState().IsFlagSet(ApplicationState::Game))
            gEditor().NeedsRedraw();

        if (_viewportCameraUI->NeedsRedraw())
            gEditor().NeedsRedraw();

        if (_isVisible && GuiAPI::Instance().IsGuiInitialized())
            ResetViewport();

        if (!ImGui::IsItemVisible() || _viewportCamera->GetRenderSettings()->OverlayOnly)
            gEditor().SetImGuizmoState(Editor::ImGuizmoState::Inactive);
        else
            gEditor().SetImGuizmoState(Editor::ImGuizmoState::Active);

        // Handle viewport gpu picking
        if (ImGui::IsWindowHovered() && gVirtualInput().IsButtonDown(_pickingBtn))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 viewportPos(mousePos.x - windowPos.x, mousePos.y - windowPos.y - 26);

            gEditor().NeedsPicking((UINT32)viewportPos.x, (UINT32)viewportPos.y);
        }

        if (gEditor().GetImGuizmoState() == Editor::ImGuizmoState::Active)
            gEditor().HandleImGuizmo();

        UpdateCameraFlag(_viewportCamera);
        HandleKeyShortcuts();
    }

    void WidgetViewport::UpdateBackground()
    {
        UINT32 flags = _viewportCamera->GetFlags();

        //We are in simulation mode, we switch to edit mode and change camera to onDemand
        if (gCoreApplication().GetState().IsFlagSet(ApplicationState::Game))
        {
            gCoreApplication().GetState().SetFlag(ApplicationState::Game, false);
            gEditor().DestroyRunningScene();

            if (!(flags & (UINT32)CameraFlag::OnDemand))
            {
                flags |= (UINT32)CameraFlag::OnDemand;
                _viewportCamera->SetFlags(flags);
            }
        }

        gEditor().SetImGuizmoState(Editor::ImGuizmoState::Inactive);
    }

    void WidgetViewport::SetVisible(bool isVisible)
    {
        Widget::SetVisible(isVisible);
        gEditor().NeedsRedraw();
    }

    void WidgetViewport::ResetViewport()
    {
        float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
        float height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

        // Viewport update is done after displaying render in widget viewport because if renderTexture needs to
        // be destroyed and built again, it won't display the render but a grey or black image
        if (CheckRenderTexture(width, height))
        {
            _viewportCamera->GetViewport()->SetTarget(_renderData.RenderTex);
            _viewportCamera->SetAspectRatio(width / height);
        }

        ImVec2 position = ImGui::GetWindowContentRegionMin();
        position.x += ImGui::GetWindowPos().x;
        position.y += ImGui::GetWindowPos().y;

        gEditor().SetImGuizmoRect(Vector2(position.x, position.y), Vector2(width, height));

        SPtr<Texture> texture = _renderData.RenderTex->GetColorTexture(0);
        //texture = gRenderer()->GetLastRenderTexture(RenderOutputType::SSAO);
        if (texture)
        {
            SPtr<TextureView> textureView = texture->RequestView(
                _renderData.ColorTexSurface.MipLevel,
                _renderData.ColorTexSurface.NumMipLevels,
                _renderData.ColorTexSurface.Face,
                _renderData.ColorTexSurface.NumFaces,
                GVU_DEFAULT, 
                texture->GetProperties().GetDebugName()
            );

            void* rawData = textureView->GetRawData();

            ImGui::Image(
                static_cast<ImTextureID>(rawData),
                ImVec2(static_cast<float>(width), static_cast<float>(height)),
                ImVec2(0, 0),
                ImVec2(1, 1)
            );
        }
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

        gRendererUtility().GenerateViewportRenderTexture(_renderData, "Viewport Output");

        _lastRenderDataUpatedTime = gTime().GetTime();
        _needResetViewport = false;

        gEditor().NeedsRedraw();

        return true;
    }

    void WidgetViewport::HandleKeyShortcuts()
    {
        if (!ImGui::IsWindowHovered() || !ImGui::IsWindowFocused() || _viewportCamera.GetInternalPtr() != gEditor().GetViewportCamera().GetInternalPtr())
            return;

        if (gVirtualInput().IsButtonDown(_copyBtn))
        {
            if (_selections.ClickedComponent)
            {
                _selections.CopiedComponent = _selections.ClickedComponent;
                gEditor().NeedsRedraw();
            }
            else
            {
                _selections.CopiedSceneObject = _selections.ClickedSceneObject;
                gEditor().NeedsRedraw();
            }
        }

        if (gVirtualInput().IsButtonDown(_pasteBtn))
            gEditor().Paste();

        if (gVirtualInput().IsButtonDown(_deleteBtn))
            gEditor().Delete();
    }

    void UpdateCameraFlag(HCamera& camera)
    {
        UINT32 flags = camera->GetFlags();
        if (!gCoreApplication().GetState().IsFlagSet(ApplicationState::Game)) //We are in editor mode
        {
            if (!(flags & (UINT32)CameraFlag::OnDemand))
            {
                flags |= (UINT32)CameraFlag::OnDemand;
                camera->SetFlags(flags);
            }
        }
        else
        {
            if (flags & (UINT32)CameraFlag::OnDemand)
            {
                flags &= ~(UINT32)CameraFlag::OnDemand;
                camera->SetFlags(flags);
            }
        }
    }
}
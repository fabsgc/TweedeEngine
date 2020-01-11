#include "TeCamera.h"
#include "Renderer/TeViewport.h"
#include "Manager/TeRendererManager.h"
#include "Scene/TeSceneManager.h"

namespace te
{
    Camera::Camera(SPtr<RenderTarget> target, float left, float top, float width, float height)
	{
		_viewport = Viewport::Create(target, left, top, width, height);
	}

	Camera::Camera(const SPtr<Viewport>& viewport)
	{
		_viewport = viewport;
	}

    Camera::~Camera()
    {
    }

    void Camera::Initialize()
    {
        gSceneManager().RegisterCamera(std::static_pointer_cast<Camera>(this->GetThisPtr()));
        RendererManager::Instance().GetRenderer()->NotifyCameraAdded(this);
    }

    void Camera::Destroy()
    {
        gSceneManager().UnregisterCamera(std::static_pointer_cast<Camera>(this->GetThisPtr()));
        RendererManager::Instance().GetRenderer()->NotifyCameraRemoved(this);

        _viewport = nullptr;
    }

    /**	Returns the viewport used by the camera. */
    void Camera::SetRenderTarget(SPtr<RenderTarget> renderTarget)
    {
        _viewport->SetTarget(renderTarget);
        RendererManager::Instance().GetRenderer()->NotifyCameraUpdated(this);
    }

    SPtr<Camera> Camera::Create()
    {
        Camera* camera = new (te_allocate<Camera>()) Camera();
        SPtr<Camera> handlerPtr = te_shared_ptr<Camera>(camera);
        handlerPtr->SetThisPtr(handlerPtr);
        handlerPtr->Initialize();

		return handlerPtr;
    }
}
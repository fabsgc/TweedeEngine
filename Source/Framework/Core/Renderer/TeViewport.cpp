#include "TeViewport.h"
#include "RenderAPI/TeRenderTarget.h"
#include "Math/TeRect2I.h"

namespace te
{
    const Color ViewportBase::DEFAULT_CLEAR_COLOR = Color(0.0f, 0.3685f, 0.7969f);

    ViewportBase::ViewportBase(float x, float y, float width, float height)
        : _normArea(x, y, width, height)
        , _clearColorValue(DEFAULT_CLEAR_COLOR)
        , _clearDepthValue(1.0f)
        , _clearStencilValue(0)
        , _clearFlags(FBT_COLOR | FBT_DEPTH | FBT_STENCIL)
    {
    }

    void ViewportBase::SetArea(const Rect2& area)
    {
        _normArea = area;
    }

    Rect2I ViewportBase::GetPixelArea() const
    {
        float width = (float)GetTargetWidth();
        float height = (float)GetTargetHeight();

        Rect2I area;
        area.x = (int)(_normArea.x * width);
        area.y = (int)(_normArea.y * height);
        area.width = (int)(_normArea.width * width);
        area.height = (int)(_normArea.height * height);

        return area;
    }

    void ViewportBase::SetClearValues(const Color& clearColor, float clearDepth, UINT16 clearStencil)
    {
        _clearColorValue = clearColor;
        _clearDepthValue = clearDepth;
        _clearStencilValue = clearStencil;
    }

    void ViewportBase::SetClearColorValue(const Color& color)
    {
        _clearColorValue = color;
    }

    void ViewportBase::SetClearDepthValue(float depth)
    {
        _clearDepthValue = depth;
    }

    void ViewportBase::SetClearStencilValue(UINT16 value)
    {
        _clearStencilValue = value;
    }

    void ViewportBase::SetClearFlags(UINT32 flags)
    {
        _clearFlags = flags;
    }

    Viewport::Viewport(const SPtr<RenderTarget>& target, float x, float y, float width, float height)
        : ViewportBase(x, y, width, height)
        , _target(target)
    {
    }

    Viewport::~Viewport()
    {
    }

    void Viewport::SetTarget(const SPtr<RenderTarget>& target)
    {
        _target = target;
    }

    UINT32 Viewport::GetTargetWidth() const
    {
        if (_target != nullptr)
            return _target->GetProperties().Width;

        return 0;
    }

    UINT32 Viewport::GetTargetHeight() const
    {
        if(_target != nullptr)
            return _target->GetProperties().Height;

        return 0;
    }

    void Viewport::Initialize()
    {
        CoreObject::Initialize();
    }

    void Viewport::Destroy()
    {
        CoreObject::Destroy();
        _target = nullptr;
    }

    SPtr<Viewport> Viewport::Create(const SPtr<RenderTarget>& target, float x, float y, float width, float height)
    {
        Viewport* viewport = new (te_allocate<Viewport>()) Viewport();
        SPtr<Viewport> handlerPtr = te_core_ptr<Viewport>(viewport);
        handlerPtr->SetThisPtr(handlerPtr);
        handlerPtr->Initialize();

        return handlerPtr;
    }
}

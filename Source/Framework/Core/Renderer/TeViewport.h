#pragma once

#include "TeCorePrerequisites.h"
#include "Math/TeRect2.h"
#include "Utility/TeColor.h"
#include "TeCoreObject.h"

namespace te
{
    /** Common base type used for both sim and core thread variants of Viewport. */
	class TE_CORE_EXPORT ViewportBase
	{
	public:
		virtual ~ViewportBase() = default;

        /** Determines the area that the viewport covers. Coordinates are in normalized [0, 1] range. */
        void SetArea(const Rect2& area);

        /** @copydoc setArea() */
        Rect2 GetArea() const { return _normArea; }

        /**	Sets values to clear color, depth and stencil buffers to. */
		void SetClearValues(const Color& clearColor, float clearDepth = 0.0f, UINT16 clearStencil = 0);

		/** Determines the color to clear the viewport to before rendering, if color clear is enabled. */
		void SetClearColorValue(const Color& color);

		/** @copydoc setClearColorValue() */
		const Color& GetClearColorValue() const { return _clearColorValue; }

		/** Determines the value to clear the depth buffer to before rendering, if depth clear is enabled. */
		void SetClearDepthValue(float depth);

		/** @copydoc setClearDepthValue() */
		float GetClearDepthValue() const { return _clearDepthValue; }

		/** Determines the value to clear the stencil buffer to before rendering, if stencil clear is enabled. */
		void SetClearStencilValue(UINT16 value);

		/** @copydoc setClearStencilValue() */
		UINT16 GetClearStencilValue() const { return _clearStencilValue; }

    protected:
		ViewportBase(float x = 0.0f, float y = 0.0f, float width = 1.0f, float height = 1.0f);

        /** Gets the render target width. */
		virtual UINT32 GetTargetWidth() const = 0;

		/**	Gets the render target width. */
		virtual UINT32 GetTargetHeight() const = 0;

    protected:
        Rect2 _normArea;

        Color _clearColorValue;
		float _clearDepthValue;
		UINT16 _clearStencilValue;

        static const Color DEFAULT_CLEAR_COLOR;
    };

    /**
	 * Viewport determines to which RenderTarget should rendering be performed. It allows you to render to a sub-region of the
	 * target by specifying the area rectangle, and allows you to set up color/depth/stencil clear values for that specific region.
	 */
	class TE_CORE_EXPORT Viewport : public ViewportBase, public CoreObject
	{
	public:
        ~Viewport();

        /**	Determines the render target the viewport is associated with. */
        void SetTarget(const SPtr<RenderTarget>& target);

        /** @copydoc setTarget() */
        SPtr<RenderTarget> GetTarget() const { return _target; }

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /** @copydoc CoreObject::Destroy */
        void Destroy() override;

        /**
		 * Creates a new viewport.
		 *
		 * @note	Viewport coordinates are normalized in [0, 1] range.
		 */
		static SPtr<Viewport> Create(const SPtr<RenderTarget>& target, float x = 0.0f, float y = 0.0f, 
            float width = 1.0f, float height = 1.0f);

    protected:
		Viewport(const SPtr<RenderTarget>& target, float x = 0.0f, float y = 0.0f, float width = 1.0f, float height = 1.0f);

        /** @copydoc ViewportBase::getTargetWidth */
		UINT32 GetTargetWidth() const override;

		/** @copydoc ViewportBase::getTargetHeight */
		UINT32 GetTargetHeight() const override;

        Viewport() = default;

    protected:
		SPtr<RenderTarget> _target;
    };
}

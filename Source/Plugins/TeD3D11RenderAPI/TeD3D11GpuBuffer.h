#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeGpuBuffer.h"
#include "TeD3D11GpuBufferView.h"

namespace te
{
    /** DirectX 11 implementation of a generic GPU buffer. */
    class D3D11GpuBuffer : public GpuBuffer
    {
    public:
        virtual ~D3D11GpuBuffer();

        /**
         * Creates a buffer view that may be used for binding a buffer to a slot in the pipeline. Views allow you to specify
         * how is data in the buffer organized to make it easier for the pipeline to interpret.
         *
         * @param[in]	buffer			Buffer to create the view for.
         * @param[in]	firstElement	Position of the first element visible by the view.
         * @param[in]	numElements		Number of elements to bind to the view.
         * @param[in]	usage			Determines type of the view we are creating, and which slots in the pipeline will
         *								the view be bindable to.
         * @param[in]	debugName		For debug purpose only
         *
         * @note If a view with this exact parameters already exists, it will be returned and new one will not be created.
         * @note Only Default and RandomWrite views are supported for this type of buffer.
         */
         // TODO Low Priority: Perhaps reflect usage flag limitation by having an enum with only the supported two options?
        static GpuBufferView* RequestView(D3D11GpuBuffer* buffer, UINT32 firstElement, UINT32 numElements,
            GpuViewUsage usage, const String& debugName);

        /**
         * Releases a view created with requestView.
         *
         * @note	View will only truly get released once all references to it are released.
         */
        static void ReleaseView(GpuBufferView* view);

        /**	Returns the internal DX11 GPU buffer object. */
        ID3D11Buffer* GetDX11Buffer() const;

        /** Returns the DX11 shader resource view object for the buffer. */
        ID3D11ShaderResourceView* GetSRV() const;

        /** Returns the DX11 unordered access view object for the buffer. */
        ID3D11UnorderedAccessView* GetUAV() const;

    protected:
        friend class D3D11HardwareBufferManager;

        D3D11GpuBuffer(const GPU_BUFFER_DESC& desc, GpuDeviceFlags deviceMask);
        D3D11GpuBuffer(const GPU_BUFFER_DESC& desc, SPtr<HardwareBuffer> underlyingBuffer);

        /**	Destroys all buffer views regardless if their reference count is zero or not. */
        void ClearBufferViews();

        /** @copydoc GpuBuffer::Initialize */
        void Initialize() override;

    private:
        /**	Helper class to help with reference counting for GPU buffer views. */
        struct GpuBufferReference
        {
            GpuBufferReference(GpuBufferView* _view)
                : View(_view)
                , RefCount(0)
            { }

            GpuBufferView* View;
            UINT32 RefCount;
        };

        GpuBufferView* _bufferView = nullptr;
        UnorderedMap<GPU_BUFFER_VIEW_DESC, GpuBufferReference*, GpuBufferView::HashFunction, GpuBufferView::EqualFunction> _bufferViews;
    };
}

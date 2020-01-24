#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"

namespace te
{
    /** Available DX11 error levels. */
    enum TE_D3D11_ERROR_LEVEL
    {
        D3D11ERR_NO_EXCEPTION,
        D3D11ERR_CORRUPTION,
        D3D11ERR_ERROR,
        D3D11ERR_WARNING,
        D3D11ERR_INFO
    };

    /** Wrapper around DirectX 11 device object. */
    class D3D11Device
    {
    public:
        /** Constructs the object with a previously created DX11 device. */
        D3D11Device(ID3D11Device* device);
        ~D3D11Device();

        /** Shuts down the device any releases any internal resources. */
        void Shutdown();

        /** Returns DX11 immediate context object. */
        ID3D11DeviceContext* GetImmediateContext() const { return _immediateContext; }

        /** Returns DX11 class linkage object. */
        ID3D11ClassLinkage* GetClassLinkage() const { return _classLinkage; }

        /** Returns internal DX11 device. */
        ID3D11Device* GetD3D11Device() const { return _D3D11Device; }

        /** Returns information about optional features supported by the device. */
        const D3D11_FEATURE_DATA_D3D11_OPTIONS& GetFeatureOptions() const { return _D3D11FeatureOptions; }

        /** Resets error state & error messages. */
        void ClearErrors();

        /**
         * Query if error occurred at any point since last clearErrors() call. Use getErrorDescription to get a string
         * describing the error.
         */
        bool HasError() const;

        /** Returns a string describing an error if one occurred. */
        String GetErrorDescription(bool clearErrors = true);
        
        /**
         * Sets the level for which we want to receive errors for. Errors will be reported for the provided level and any
         * higher priority level.
         */
        void SetExceptionsErrorLevel(const TE_D3D11_ERROR_LEVEL exceptionsErrorLevel);

    private:
        D3D11Device() = default;

        ID3D11Device* _D3D11Device = nullptr;
        ID3D11DeviceContext* _immediateContext = nullptr;
        ID3D11InfoQueue* _infoQueue = nullptr;
        ID3D11ClassLinkage* _classLinkage = nullptr;
        D3D11_FEATURE_DATA_D3D11_OPTIONS _D3D11FeatureOptions;
    };
}
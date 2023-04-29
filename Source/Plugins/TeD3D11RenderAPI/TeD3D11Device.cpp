#include "TeD3D11Device.h"

namespace te
{
    D3D11Device::D3D11Device(ID3D11Device* device, bool useDebugLayer)
        : _D3D11Device(device)
        , _D3D11FeatureOptions()
    {
        assert(device != nullptr);

        if (device)
        {
            device->GetImmediateContext(&_immediateContext);

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
            if (useDebugLayer)
            {
                // This interface is not available unless we created the device with debug layer
                HRESULT hr = _D3D11Device->QueryInterface(__uuidof(ID3D11InfoQueue), (LPVOID*)&_infoQueue);

                if (FAILED(hr))
                    TE_ASSERT_ERROR(false, "Unable to query D3D11InfoQueue");

                SetExceptionsErrorLevel(D3D11ERR_WARNING);
            }
#endif

            // If feature level is 11, create class linkage
            SAFE_RELEASE(_classLinkage);
            if (_D3D11Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)
            {
                HRESULT hr = _D3D11Device->CreateClassLinkage(&_classLinkage);

                if (FAILED(hr))
                {
                    TE_ASSERT_ERROR(false, "Unable to create class linkage.");
                }
            }

            // Get feature options
            _D3D11Device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS, &_D3D11FeatureOptions, sizeof(_D3D11FeatureOptions));
        }
    }

    D3D11Device::~D3D11Device()
    {
        Shutdown();
    }

    void D3D11Device::Shutdown()
    {
        if (_immediateContext)
        {
            _immediateContext->Flush();
            _immediateContext->ClearState();
        }

        SAFE_RELEASE(_infoQueue);
        SAFE_RELEASE(_D3D11Device);
        SAFE_RELEASE(_immediateContext);
        SAFE_RELEASE(_classLinkage);
    }

    String D3D11Device::GetErrorDescription(bool doClearErrors)
    {
        if (_D3D11Device == nullptr)
            return "Null device.";

        String res;

#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        if (_infoQueue != nullptr)
        {
            UINT64 numStoredMessages = _infoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
            for (UINT64 i = 0 ; i < numStoredMessages ; i++ )
            {
                // Get the size of the message
                SIZE_T messageLength = 0;
                _infoQueue->GetMessage(i, nullptr, &messageLength);
                // Allocate space and get the message
                D3D11_MESSAGE* pMessage = (D3D11_MESSAGE*)te_allocate(static_cast<UINT32>(messageLength));
                _infoQueue->GetMessage(i, pMessage, &messageLength);
                res = res + pMessage->pDescription + "\n";
                res = res + ToString(pMessage->ID) + "\n";
                te_deallocate(pMessage);
            }
        }

        if(doClearErrors)
        {
            ClearErrors();
        }
#endif

        return res;
    }

    bool D3D11Device::HasError() const
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        if (_infoQueue != nullptr)
        {
            const UINT64 numStoredMessages = _infoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
            return numStoredMessages > 0;
        }
#endif

        return false;
    }

    void D3D11Device::ClearErrors()
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        if (_D3D11Device != nullptr && _infoQueue != nullptr)
        {
            _infoQueue->ClearStoredMessages();
        }
#endif
    }

    void D3D11Device::SetExceptionsErrorLevel(const TE_D3D11_ERROR_LEVEL exceptionsErrorLevel)
    {
#if TE_DEBUG_MODE == TE_DEBUG_ENABLED
        if(_infoQueue == nullptr)
            return;

        _infoQueue->ClearRetrievalFilter();
        _infoQueue->ClearStorageFilter();

        D3D11_INFO_QUEUE_FILTER filter;
        ZeroMemory(&filter, sizeof(D3D11_INFO_QUEUE_FILTER));
        Vector<D3D11_MESSAGE_SEVERITY> severityList;

        switch(exceptionsErrorLevel)
        {
            case D3D11ERR_INFO:
                severityList.push_back(D3D11_MESSAGE_SEVERITY_INFO);
            case D3D11ERR_WARNING:
                severityList.push_back(D3D11_MESSAGE_SEVERITY_WARNING);
            case D3D11ERR_ERROR:
                severityList.push_back(D3D11_MESSAGE_SEVERITY_ERROR);
            case D3D11ERR_CORRUPTION:
                severityList.push_back(D3D11_MESSAGE_SEVERITY_CORRUPTION);
            case D3D11ERR_NO_EXCEPTION:
            default:
            break;
        }

        if (severityList.size() > 0)
        {
            filter.AllowList.NumSeverities = (UINT)severityList.size();
            filter.AllowList.pSeverityList = &severityList[0];
        }

        D3D11_MESSAGE_ID knownMessages[] =
        {
            D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
            D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET
        };

        filter.DenyList.NumIDs = _countof(knownMessages);
        filter.DenyList.pIDList = knownMessages;

        _infoQueue->AddStorageFilterEntries(&filter);
        _infoQueue->AddRetrievalFilterEntries(&filter);

        _infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
        _infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
    }
}

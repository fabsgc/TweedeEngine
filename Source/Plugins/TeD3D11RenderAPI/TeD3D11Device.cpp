#include "TeD3D11Device.h"

namespace te
{
    D3D11Device::D3D11Device(ID3D11Device* device)
		: _D3D11Device(device)
        , _D3D11FeatureOptions()
	{
        assert(device != nullptr);
        // TODO

		if (device)
		{
            device->GetImmediateContext(&_immediateContext);

#if TE_DEBUG_MODE
            // This interface is not available unless we created the device with debug layer
			HRESULT hr = _D3D11Device->QueryInterface(__uuidof(ID3D11InfoQueue), (LPVOID*)&_infoQueue);

			if (FAILED(hr))
            {
                TE_ASSERT_ERROR(false, "Unable to query D3D11InfoQueue", __FILE__, __LINE__);
            }

			SetExceptionsErrorLevel(D3D11ERR_ERROR);
#endif

            // If feature level is 11, create class linkage
			SAFE_RELEASE(_classLinkage);
			if (_D3D11Device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0)
			{
				HRESULT hr = _D3D11Device->CreateClassLinkage(&_classLinkage);

				if (FAILED(hr))
                {
                    TE_ASSERT_ERROR(false, "Unable to create class linkage.", __FILE__, __LINE__);
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

		if (_infoQueue != nullptr)
		{
			UINT64 numStoredMessages = _infoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
			for (UINT64 i = 0 ; i < numStoredMessages ; i++ )
			{
				// Get the size of the message
				SIZE_T messageLength = 0;
				_infoQueue->GetMessage(i, nullptr, &messageLength);
				// Allocate space and get the message
				D3D11_MESSAGE* pMessage = (D3D11_MESSAGE*)malloc(messageLength);
				_infoQueue->GetMessage(i, pMessage, &messageLength);
				res = res + pMessage->pDescription + "\n";
				free(pMessage);
			}
		}

		if(doClearErrors)
        {
            ClearErrors();
        }

		return res;
    }

    bool D3D11Device::HasError() const
	{
        if (_infoQueue != nullptr)
		{
			const UINT64 numStoredMessages = _infoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
			return numStoredMessages > 0;
		}

		return false;
    }

    void D3D11Device::ClearErrors()
	{
        if (_D3D11Device != nullptr && _infoQueue != nullptr)
        {
            _infoQueue->ClearStoredMessages();
        }
    }

    void D3D11Device::SetExceptionsErrorLevel(const TE_D3D11_ERROR_LEVEL exceptionsErrorLevel)
	{
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

		_infoQueue->AddStorageFilterEntries(&filter);
		_infoQueue->AddRetrievalFilterEntries(&filter);
    }
}
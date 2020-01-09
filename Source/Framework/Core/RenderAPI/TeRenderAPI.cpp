#include "TeRenderAPI.h"
#include "RenderAPI/TeRenderWindow.h"

namespace te
{
    TE_MODULE_STATIC_MEMBER(RenderAPI)

	RenderAPI::RenderAPI()
	{
	}

	RenderAPI::~RenderAPI()
	{
	}

    void RenderAPI::Initialize()
    {
        // Do nothing
    }

    void RenderAPI::Destroy()
    {
        _activeRenderTarget = nullptr;
    }
}
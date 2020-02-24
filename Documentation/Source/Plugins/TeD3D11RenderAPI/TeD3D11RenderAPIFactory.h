#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderAPIFactory.h"

namespace te
{
    class D3D11RenderAPIFactory : public RenderAPIFactory
    {
    public:
        static constexpr const char* SystemName = "TeD3D11RenderAPI";

        void Create() override;

        const String& Name() const override;
    };
}
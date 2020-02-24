#pragma once

#include "TeGLRenderAPIPrerequisites.h"
#include "RenderAPI/TeRenderAPIFactory.h"

namespace te
{
    class GLRenderAPIFactory : public RenderAPIFactory
    {
    public:
        static constexpr const char* SystemName = "TeGLRenderAPI";

        void Create() override;

        const String& Name() const override;
    };
}
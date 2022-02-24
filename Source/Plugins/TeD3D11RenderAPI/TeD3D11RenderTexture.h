#pragma once

#include "TeD3D11RenderAPIPrerequisites.h"
#include "Image/TeTexture.h"
#include "RenderAPI/TeRenderTexture.h"

namespace te
{
    class D3D11RenderTexture : public RenderTexture
    {
    public:
        D3D11RenderTexture(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx);
        virtual ~D3D11RenderTexture() { }

        /** @copydoc RenderTexture::GetCustomAttribute */
        void GetCustomAttribute(const String& name, void* data) const override;

    protected:
        friend class D3D11TextureManager;

        D3D11RenderTexture(const RENDER_TEXTURE_DESC& desc);

        /** @copydoc RenderTexture::GetProperties */
        const RenderTargetProperties& GetProperties() const override { return _properties; }

        RenderTextureProperties _properties;
    };
}

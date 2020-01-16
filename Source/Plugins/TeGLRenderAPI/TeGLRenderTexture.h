#include "TeGLRenderAPIPrerequisites.h"
#include "Image/TeTexture.h"
#include "RenderAPI/TeRenderTexture.h"

namespace te
{
    class GLRenderTexture : public RenderTexture
	{
	public:
		GLRenderTexture(const RENDER_TEXTURE_DESC& desc, UINT32 deviceIdx);
		virtual ~GLRenderTexture() { }

		/** @copydoc RenderTexture::GetCustomAttribute */
		void GetCustomAttribute(const String& name, void* data) const override;

	protected:
		friend class GLTextureManager;

		GLRenderTexture(const RENDER_TEXTURE_DESC& desc);

		/** @copydoc RenderTexture::GetProperties */
		const RenderTargetProperties& GetProperties() const override { return _properties; }

		RenderTextureProperties _properties;
	};
}

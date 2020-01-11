#include "TeRenderManPrerequisites.h"
#include "Renderer/TeRenderer.h"

namespace te
{
    /** Contains most scene objects relevant to the renderer. */
	struct SceneInfo
	{
        // Cameras and render targets
		Vector<RendererRenderTarget> RenderTargets;
    };

    /** Contains information about the scene (e.g. renderables, lights, cameras) required by the renderer. */
	class RendererScene
	{
	public:
		RendererScene(const SPtr<RenderManOptions>& options);
		~RendererScene();

        /** Returns a modifiable version of SceneInfo. Only to be used by friends who know what they are doing. */
		SceneInfo& GetSceneInfo() { return _info; }

        /** Registers a new camera in the scene. */
        void RegisterCamera(Camera* camera);

        /** Updates information about a previously registered camera. */
        void UpdateCamera(Camera* camera);

        /** Removes a camera from the scene. */
        void UnregisterCamera(Camera* camera);

    private:
        SceneInfo _info;
        SPtr<RenderManOptions> _options;

    };
}

#include "TeGLBlendState.h"
#include "TeGLRenderAPI.h"
namespace te
{
	GLBlendState::GLBlendState(const BLEND_STATE_DESC& desc)
		: BlendState(desc)
	{ }

	GLBlendState::~GLBlendState()
	{ }

	void GLBlendState::CreateInternal()
	{
		// TODO
		BlendState::CreateInternal();
	}
}

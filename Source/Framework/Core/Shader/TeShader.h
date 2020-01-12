#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
	struct TE_CORE_EXPORT SHADER_DESC
	{
		SHADER_DESC() {}
	};

	class TE_CORE_EXPORT Shader : public Resource
	{
	public:
		Shader();
		~Shader();

		/**	Creates a new shader resource using the provided descriptor and techniques. */
		static HShader Create(const SHADER_DESC& desc);

		/**	Returns a shader object but doesn't initialize it. */
		static SPtr<Shader> CreateEmpty();

		/**
		 * Creates a new shader object using the provided descriptor and techniques.
		 *
		 * @note	Internal method. Use create() for normal use.
		 */
		static SPtr<Shader> _createPtr(const SHADER_DESC& desc);

		/** @copydoc CoreObject::Initialize */
		void Initialize() override;

	private:
		Shader(const SHADER_DESC& desc);

	protected:
		SHADER_DESC _desc;
    };
}

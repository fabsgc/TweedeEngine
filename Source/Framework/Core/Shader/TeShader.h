#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"

namespace te
{
	struct TE_CORE_EXPORT SHADER_DESC
	{
        /**
         * Sorting type to use when performing sort in the render queue. Default value is sort front to back which causes
         * least overdraw and is preferable. Transparent objects need to be sorted back to front. You may also specify no
         * sorting and the elements will be rendered in the order they were added to the render queue.
         */
        QueueSortType QueueType;

        /**
         * Priority that allows you to control in what order are your shaders rendered. See QueuePriority for a list of
         * initial values. Shaders with higher priority will be rendered before shaders with lower priority, and
         * additionally render queue will only sort elements within the same priority group.
         *
         * @note
         * This is useful when you want all your opaque objects to be rendered before you start drawing your transparent
         * ones. Or to render your overlays after everything else. Values provided in QueuePriority are just for general
         * guidance and feel free to increase them or decrease them for finer tuning. (for example QueuePriority::Opaque +
         * 1).
         */
        INT32 QueuePriority;

        /**
         * Enables or disables separable passes. When separable passes are disabled all shader passes will be executed in a
         * sequence one after another. If it is disabled the renderer is free to mix and match passes from different
         * objects to achieve best performance. (They will still be executed in sequence, but some other object may be
         * rendered in-between passes)
         *
         * @note	Shaders with transparency generally can't be separable, while opaque can.
         */
        bool SeparablePasses;

        /** Flags that let the renderer know how should it interpret the shader. */
        ShaderFlag Flag;
	};

	class TE_CORE_EXPORT Shader : public Resource
	{
	public:
		~Shader();

		/**	Creates a new shader resource using the provided descriptor and techniques. */
		static HShader Create(const String& name, const SHADER_DESC& desc);

		/**	Returns a shader object but doesn't initialize it. */
		static SPtr<Shader> CreateEmpty();

		/**
		 * Creates a new shader object using the provided descriptor and techniques.
		 *
		 * @note	Internal method. Use create() for normal use.
		 */
		static SPtr<Shader> _createPtr(const String& name, const SHADER_DESC& desc);

		/** @copydoc CoreObject::Initialize */
		void Initialize() override;

        /** Returns the unique shader ID. */
        UINT32 GetId() const { return _id; }

        /** Returns the shader name. */
        const String& GetName() const { return _name; }

        /** Set shader name */
        void SetName(const String& name) { _name = name; }

	private:
        Shader();
        Shader(UINT32 id);
		Shader(const SHADER_DESC& desc, const String& name, UINT32 id);

	protected:
		SHADER_DESC _desc;
        String _name;
        UINT32 _id;

        static std::atomic<UINT32> NextShaderId;
    };
}

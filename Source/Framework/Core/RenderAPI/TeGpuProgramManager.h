#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"
#include "RenderAPI/TeGpuProgram.h"

namespace te
{
    /** Factory responsible for creating GPU programs of a certain type. */
    class TE_CORE_EXPORT GpuProgramFactory
    {
    public:
        GpuProgramFactory() = default;
        virtual ~GpuProgramFactory() = default;

        /** @copydoc GpuProgram::Create */
        virtual SPtr<GpuProgram> Create(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT) = 0;

        /**
         * Creates a completely empty and uninitialized GpuProgram. Should only be used for specific purposes, like
         * deserialization, as it requires additional manual initialization that is not required normally.
         */
        virtual SPtr<GpuProgram> Create(GpuProgramType type, GpuDeviceFlags deviceMask = GDF_DEFAULT) = 0;

        /** @copydoc GpuProgram::CompileBytecode */
        virtual SPtr<GpuProgramBytecode> CompileBytecode(const GPU_PROGRAM_DESC& desc) = 0;
    };

    /**
     * Manager responsible for creating GPU programs. It will automatically	try to find the appropriate handler for a
     * specific GPU program language and create the program if possible.
     *
     * @note	Core thread only unless otherwise specified.
     */
    class TE_CORE_EXPORT GpuProgramManager : public Module<GpuProgramManager>
    {
    public:
        GpuProgramManager();
        virtual ~GpuProgramManager();

        TE_MODULE_STATIC_HEADER_MEMBER(GpuProgramManager)

        /**
         * Registers a new factory that is able to create GPU programs for a certain language. If any other factory for the
         * same language exists, it will overwrite it.
         */
        void AddFactory(const String& language, GpuProgramFactory* factory);

        /**
         * Unregisters a GPU program factory, essentially making it not possible to create GPU programs using the language
         * the factory supported.
         */
        void RemoveFactory(const String& language);

        /** Query if a GPU program language is supported (for example "hlsl", "glsl"). Thread safe. */
        bool IsLanguageSupported(const String& language);

        /** @copydoc GpuProgram::create */
        SPtr<GpuProgram> Create(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /** @copydoc GpuProgram::compileBytecode */
        SPtr<GpuProgramBytecode> CompileBytecode(const GPU_PROGRAM_DESC& desc);

    protected:
        friend class GpuProgram;

        /**
         * Creates a GPU program without initializing it.
         *
         * @see		create
         */
        SPtr<GpuProgram> CreateInternal(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /** Attempts to find a factory for the specified language. Returns null if it cannot find one. */
        GpuProgramFactory* GetFactory(const String& language);

    protected:
        UnorderedMap<String, GpuProgramFactory*> _factories;
        GpuProgramFactory* _nullFactory; /**< Factory for dealing with GPU programs that can't be created. */
    };

    /**	Factory that creates null GPU programs.  */
    class TE_CORE_EXPORT NullProgramFactory : public GpuProgramFactory
    {
    public:
        NullProgramFactory() = default;
        ~NullProgramFactory() = default;

        SPtr<GpuProgram> Create(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask) override;
        SPtr<GpuProgram> Create(GpuProgramType type, GpuDeviceFlags deviceMask) override;
        SPtr<GpuProgramBytecode> CompileBytecode(const GPU_PROGRAM_DESC& desc) override;
    };
}

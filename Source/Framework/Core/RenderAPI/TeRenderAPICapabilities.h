#pragma once

#include "TeCorePrerequisites.h"
#include "RenderAPI/TeGpuProgram.h"

#include <cstdint>
#define CAPS_CATEGORY_SIZE INT64_C(8)
#define TE_CAPS_BITSHIFT (INT64_C(64) - CAPS_CATEGORY_SIZE)
#define CAPS_CATEGORY_MASK (((INT64_C(1) << CAPS_CATEGORY_SIZE) - INT64_C(1)) << TE_CAPS_BITSHIFT)
#define TE_CAPS_VALUE(cat, val) ((cat << TE_CAPS_BITSHIFT) | (INT64_C(1) << val))

#define TE_MAX_BOUND_VERTEX_BUFFERS 16

namespace te
{
    /** Categories of render API capabilities. */
    enum CapabilitiesCategory : UINT64
    {
        CAPS_CATEGORY_COMMON = 0,
        CAPS_CATEGORY_GL = 1,
        CAPS_CATEGORY_D3D11 = 2,
        CAPS_CATEGORY_VULKAN = 3,
        CAPS_CATEGORY_COUNT = 32 /**< Maximum number of categories. */
    };

    /** Enum describing the different hardware capabilities we can check for. */
    enum Capabilities : UINT64
    {
        /** Supports compressed textures in the BC formats. */
        RSC_TEXTURE_COMPRESSION_BC		= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 0),
        /** Supports compressed textures in the ETC2 and EAC format. */
        RSC_TEXTURE_COMPRESSION_ETC2	= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 1),
        /** Supports compressed textures in the ASTC format. */
        RSC_TEXTURE_COMPRESSION_ASTC	= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 2),
        /** Supports GPU geometry programs. */
        RSC_GEOMETRY_PROGRAM			= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 3),
        /** Supports GPU tessellation programs. */
        RSC_TESSELLATION_PROGRAM		= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 4),
        /** Supports GPU compute programs. */
        RSC_COMPUTE_PROGRAM				= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 5),
        /** Supports load-store (unordered access) writes to textures/buffers in GPU programs. */
        RSC_LOAD_STORE					= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 6),
        /** Supports load-store (unordered access) writes to textures with multiple samples. */
        RSC_LOAD_STORE_MSAA				= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 7),
        /**
         * Supports views that allow a sub-set of a texture to be bound to a GPU program. (i.e. specific mip level or mip
         * range, and/or specific array slice or array slice range)
         */
        RSC_TEXTURE_VIEWS				= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 8),
        /** GPU programs are allowed to cache their bytecode for faster compilation. */
        RSC_BYTECODE_CACHING			= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 9),
        /** Supports rendering to multiple layers of a render texture at once. */
        RSC_RENDER_TARGET_LAYERS		= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 10),
        /** Has native support for command buffers that can be populated from secondary threads. */
        RSC_MULTI_THREADED_CB			= TE_CAPS_VALUE(CAPS_CATEGORY_COMMON, 11),
    };

    /** Conventions used for a specific render backend. */
    struct TE_CORE_EXPORT Conventions
    {
        enum class Axis : UINT8
        {
            Up, Down
        };

        enum class MatrixOrder : UINT8
        {
            ColumnMajor, RowMajor
        };

        /** Determines the direction of the Y axis in UV (texture mapping) space. */
        Axis UV_YAxis = Axis::Down;

        /** Determines the direction of the Y axis in normalized device coordinate (NDC) space. */
        Axis NDC_YAxis = Axis::Up;

        /** Determines the order in which matrices are laid out in GPU programs. */
        MatrixOrder matrixOrder = MatrixOrder::RowMajor;
    };

    /** Holds data about render system driver version. */
    struct TE_CORE_EXPORT DriverVersion
    {
        DriverVersion() = default;

        /**	Returns the driver version as a single string. */
        String ToString() const
        {
            StringStream str;
            str << major << "." << minor << "." << release << "." << build;
            return str.str();
        }

        /** Parses a string in the major.minor.release.build format and stores the version numbers. */
        void FromString(const String& versionString)
        {
            Vector<String> tokens = Split(versionString, '.');
            if(!tokens.empty())
            {
                major = ParseINT32(tokens[0]);
                if (tokens.size() > 1)
                    minor = ParseINT32(tokens[1]);
                if (tokens.size() > 2)
                    release = ParseINT32(tokens[2]);
                if (tokens.size() > 3)
                    build = ParseINT32(tokens[3]);
            }

        }

        INT32 major = 0;
        INT32 minor = 0;
        INT32 release = 0;
        INT32 build = 0;
    };

    /** Types of GPU vendors. */
    enum GPUVendor
    {
        GPU_UNKNOWN = 0,
        GPU_NVIDIA = 1,
        GPU_AMD = 2,
        GPU_INTEL = 3,
        GPU_VENDOR_COUNT = 4
    };

    /** Information about hardware (GPU) and driver capabilities, such as supported features, limits and conventions. */
    class TE_CORE_EXPORT RenderAPICapabilities final
    {
    public:
        /** The identifier associated with the render API. */
        String RenderAPIName;

        /** Current version of the driver (render backend). */
        DriverVersion Driver;

        /** The name of the device (GPU) as reported by the render system. */
        String DeviceName;

        /** Vendor of the device (GPU). */
        GPUVendor DeviceVendor = GPU_UNKNOWN;

        /** The number of texture units available per stage. */
        UINT16 NumTextureUnitsPerStage[GPT_COUNT] { 0 };

        /** Total number of texture units available. */
        UINT16 NumCombinedTextureUnits = 0;

        /** The number of parameter block buffers available per stage. */
        UINT16 NumGpuParamBlockBuffersPerStage[GPT_COUNT] { 0 };

        /** Total number of parameter block buffers available. */
        UINT16 NumCombinedParamBlockBuffers = 0;

        /** The number of load-store texture unitss available per stage. */
        UINT16 NumLoadStoreTextureUnitsPerStage[GPT_COUNT] { 0 };

        /** Total number of load-store texture units available. */
        UINT16 NumCombinedLoadStoreTextureUnits = 0;

        /** Maximum number of vertex buffers we can bind at once. */
        UINT32 MaxBoundVertexBuffers = 0;

        /** The number of simultaneous render targets supported. */
        UINT16 NumMultiRenderTargets = 0;

        /** The number of vertices a geometry program can emit in a single run. */
        UINT32 GeometryProgramNumOutputVertices = 0;

        /** Horizontal texel offset used for mapping texels to pixels. */
        float HorizontalTexelOffset = 0.0f;

        /** Vertical texel offset used for mapping texels to pixels. */
        float VerticalTexelOffset = 0.0f;

        /** Minimum (closest) depth value used by this render backend */
        float MinDepth = 0.0f;

        /** Maximum (farthest) depth value used by this render backend. */
        float MaxDepth = 1.0f;

        /** Returns various conventions expected by the render backend. */
        Conventions Convention;

        /** Native type used for vertex colors. */
        VertexElementType VertexColorType = VET_COLOR_ABGR;

        /**	Sets a capability flag indicating this capability is supported. */
        void SetCapability(const Capabilities c)
        {
            UINT64 index = (CAPS_CATEGORY_MASK & c) >> TE_CAPS_BITSHIFT;
            _capabilities[index] |= (c & ~CAPS_CATEGORY_MASK);
        }

        /**	Remove a capability flag indicating this capability is not supported (default). */
        void UnsetCapability(const Capabilities c)
        {
            UINT64 index = (CAPS_CATEGORY_MASK & c) >> TE_CAPS_BITSHIFT;
            _capabilities[index] &= (~c | CAPS_CATEGORY_MASK);
        }

        /**	Checks is the specified capability supported. */
        bool HasCapability(const Capabilities c) const
        {
            UINT64 index = (CAPS_CATEGORY_MASK & c) >> TE_CAPS_BITSHIFT;
            return (_capabilities[index] & (c & ~CAPS_CATEGORY_MASK)) != 0;
        }

        /**	Adds a shader profile to the list of render-system specific supported profiles. */
        void AddShaderProfile(const String& profile)
        {
            _supportedShaderProfiles.insert(profile);
        }

        /**	Returns true if the provided profile is supported. */
        bool IsShaderProfileSupported(const String& profile) const
        {
            return (_supportedShaderProfiles.end() != _supportedShaderProfiles.find(profile));
        }

        /**	Returns a set of all supported shader profiles. */
        const UnorderedSet<String>& GetSupportedShaderProfiles() const
        {
            return _supportedShaderProfiles;
        }

        /** Parses a vendor string and returns an enum with the vendor if parsed succesfully. */
        static GPUVendor VendorFromString(const String& vendorString);

        /** Converts a vendor enum to a string. */
        static String VendorToString(GPUVendor vendor);

    private:
        static char const * const GPU_VENDOR_STRINGS[GPU_VENDOR_COUNT];

        /** Stores the capabilities flags. */
        UINT32 _capabilities[CAPS_CATEGORY_COUNT] { 0 };

        /** The list of supported shader profiles. */
        UnorderedSet<String> _supportedShaderProfiles;
    };
}

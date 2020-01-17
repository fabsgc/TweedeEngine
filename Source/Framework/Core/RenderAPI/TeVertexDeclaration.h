#pragma once

#include "TeCorePrerequisites.h"
#include "CoreUtility/TeCoreObject.h"

namespace te
{
    /** Semantics that are used for identifying the meaning of vertex buffer elements. */
    enum VertexElementSemantic
    {
        VES_POSITION = 1, /**< Position */
        VES_BLEND_WEIGHTS = 2, /**< Blend weights */
        VES_BLEND_INDICES = 3, /**< Blend indices */
        VES_NORMAL = 4, /**< Normal */
        VES_COLOR = 5, /**< Color */
        VES_TEXCOORD = 6, /**< UV coordinate */
        VES_BITANGENT = 7, /**< Bitangent */
        VES_TANGENT = 8, /**< Tangent */
        VES_POSITIONT = 9, /**< Transformed position */
        VES_PSIZE = 10 /**< Point size */
    };

    /**	Types used to identify base types of vertex element contents. */
    enum VertexElementType
    {
        VET_FLOAT1 = 0, /**< 1D floating point value */
        VET_FLOAT2 = 1, /**< 2D floating point value */
        VET_FLOAT3 = 2, /**< 3D floating point value */
        VET_FLOAT4 = 3, /**< 4D floating point value */
        VET_COLOR = 4, /**< Color encoded in 32-bits (8-bits per channel). */
        VET_SHORT1 = 5, /**< 1D 16-bit signed integer value */
        VET_SHORT2 = 6, /**< 2D 16-bit signed integer value */
        VET_SHORT4 = 8, /**< 4D 16-bit signed integer value */
        VET_UBYTE4 = 9, /**< 4D 8-bit unsigned integer value */
        VET_COLOR_ARGB = 10, /**< Color encoded in 32-bits (8-bits per channel) in ARGB order) */
        VET_COLOR_ABGR = 11, /**< Color encoded in 32-bits (8-bits per channel) in ABGR order) */
        VET_UINT4 = 12, /**< 4D 32-bit unsigned integer value */
        VET_INT4 = 13,  /**< 4D 32-bit signed integer value */
        VET_USHORT1 = 14, /**< 1D 16-bit unsigned integer value */
        VET_USHORT2 = 15, /**< 2D 16-bit unsigned integer value */
        VET_USHORT4 = 17, /**< 4D 16-bit unsigned integer value */
        VET_INT1 = 18,  /**< 1D 32-bit signed integer value */
        VET_INT2 = 19,  /**< 2D 32-bit signed integer value */
        VET_INT3 = 20,  /**< 3D 32-bit signed integer value */
        VET_UINT1 = 21,  /**< 1D 32-bit signed integer value */
        VET_UINT2 = 22,  /**< 2D 32-bit signed integer value */
        VET_UINT3 = 23,  /**< 3D 32-bit signed integer value */
        VET_UBYTE4_NORM = 24, /**< 4D 8-bit unsigned integer interpreted as a normalized value in [0, 1] range. */
        VET_COUNT, // Keep at end before VET_UNKNOWN
        VET_UNKNOWN = 0xffff
    };

    /**	Describes a single vertex element in a vertex declaration. */
    class TE_CORE_EXPORT VertexElement
    {
    public:
        VertexElement() = default;
        VertexElement(UINT16 source, UINT32 offset, VertexElementType theType,
            VertexElementSemantic semantic, UINT16 index = 0, UINT32 instanceStepRate = 0);

        bool operator== (const VertexElement & rhs) const;
        bool operator!= (const VertexElement & rhs) const;

        /**	Returns index of the vertex buffer from which this element is stored. */
        UINT16 GetStreamIdx() const { return _source; }

        /**
         * Returns an offset into the buffer where this vertex is stored. This value might be in bytes but doesn't have to
         * be, it's likely to be render API specific.
         */
        UINT32 GetOffset() const { return _offset; }

        /** Gets the base data type of this element. */
        VertexElementType GetType() const { return _type; }

        /**	Gets a semantic that describes what this element contains. */
        VertexElementSemantic GetSemantic() const { return _semantic; }

        /**
         * Gets an index of this element. Only relevant when you have multiple elements with the same semantic,
         * for example uv0, uv1.
         */
        UINT16 GetSemanticIdx() const { return _index; }

        /** Returns the size of this element in bytes. */
        UINT32 GetSize() const;

        /**
         * Returns at what rate do the vertex elements advance during instanced rendering. Provide zero for default
         * behaviour where each vertex receives the next value from the vertex buffer. Provide a value larger than zero
         * to ensure vertex data is advanced with every instance, instead of every vertex (for example a value of 1 means
         * each instance will retrieve a new value from the vertex buffer, a value of 2 means each second instance will,
         * etc.).
         */
        UINT32 GetInstanceStepRate() const { return _instanceStepRate; }

        /**	Returns the size of a base element type. */
        static UINT32 GetTypeSize(VertexElementType etype);

        /** Returns the number of values in the provided base element type. For example float4 has four values. */
        static UINT16 GetTypeCount(VertexElementType etype);

        /**	Gets packed color vertex element type used by the active render system. */
        static VertexElementType GetBestColorVertexElementType();

        /** Calculates a hash value for the provided vertex element. */
        static size_t GetHash(const VertexElement & element);
    protected:
        UINT16 _source;
        UINT32 _offset;
        VertexElementType _type;
        VertexElementSemantic _semantic;
        UINT16 _index;
        UINT32 _instanceStepRate;
    };

    /**
     * Describes a set of vertex elements, used for describing contents of a vertex buffer or inputs to a vertex GPU program.
     */
    class TE_CORE_EXPORT VertexDeclaration : public CoreObject
    {
    public:
        virtual ~VertexDeclaration() { }

    protected:
        friend class HardwareBufferManager;
    };
}

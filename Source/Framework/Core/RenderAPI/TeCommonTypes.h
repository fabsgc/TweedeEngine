#pragma once

namespace te
{
    /**	Factors used when blending new pixels with existing pixels. */
    enum BlendFactor
    {
        BF_ONE, /**< Use a value of one for all pixel components. */
        BF_ZERO, /**< Use a value of zero for all pixel components. */
        BF_DEST_COLOR, /**< Use the existing pixel value. */
        BF_SOURCE_COLOR, /**< Use the newly generated pixel value. */
        BF_INV_DEST_COLOR, /**< Use the inverse of the existing value. */
        BF_INV_SOURCE_COLOR, /**< Use the inverse of the newly generated pixel value. */
        BF_DEST_ALPHA, /**< Use the existing alpha value. */
        BF_SOURCE_ALPHA, /**< Use the newly generated alpha value. */
        BF_INV_DEST_ALPHA, /**< Use the inverse of the existing alpha value. */
        BF_INV_SOURCE_ALPHA /**< Use the inverse of the newly generated alpha value. */
    };

    /**	Operations that determines how are blending factors combined. */
    enum BlendOperation
    {
        BO_ADD, /**< Blend factors are added together. */
        BO_SUBTRACT, /**< Blend factors are subtracted in "srcFactor - dstFactor" order. */
        BO_REVERSE_SUBTRACT, /**< Blend factors are subtracted in "dstFactor - srcFactor" order. */
        BO_MIN, /**< Minimum of the two factors is chosen. */
        BO_MAX /**< Maximum of the two factors is chosen. */
    };

	/**	Comparison functions used for the depth/stencil buffer. */
	enum CompareFunction
	{
		CMPF_ALWAYS_FAIL, /**< Operation will always fail. */
		CMPF_ALWAYS_PASS, /**< Operation will always pass. */
		CMPF_LESS, /**< Operation will pass if the new value is less than existing value. */
		CMPF_LESS_EQUAL, /**< Operation will pass if the new value is less or equal than existing value. */
		CMPF_EQUAL, /**< Operation will pass if the new value is equal to the existing value. */
		CMPF_NOT_EQUAL, /**< Operation will pass if the new value is not equal to the existing value. */
		CMPF_GREATER_EQUAL, /**< Operation will pass if the new value greater or equal than the existing value. */
		CMPF_GREATER /**< Operation will pass if the new value greater than the existing value. */
	};
	
    /**	Types of frame buffers. */
	enum FrameBufferType
	{
		FBT_COLOR = 0x1, /**< Color surface. */
		FBT_DEPTH = 0x2, /**< Depth surface. */
		FBT_STENCIL = 0x4 /**< Stencil surface. */
	};

	/**
	 * Types of culling that determine how (and if) hardware discards faces with certain winding order. Winding order can
	 * be used for determining front or back facing polygons by checking the order of its vertices from the render
	 * perspective.
	 */
	enum CullingMode
	{
		CULL_NONE = 0, /**< Hardware performs no culling and renders both sides. */
		CULL_CLOCKWISE = 1, /**< Hardware culls faces that have a clockwise vertex ordering. */
		CULL_COUNTERCLOCKWISE = 2 /**< Hardware culls faces that have a counter-clockwise vertex ordering. */
	};

	/**	Polygon mode to use when rasterizing. */
	enum PolygonMode
	{
		PM_WIREFRAME = 1, /**< Render as wireframe showing only polygon outlines. */
		PM_SOLID = 2 /**< Render as solid showing whole polygons. */
	};

    /**	Types of action that can happen on the stencil buffer. */
    enum StencilOperation
    {
        SOP_KEEP, /**< Leave the stencil buffer unchanged. */
        SOP_ZERO, /**< Set the stencil value to zero. */
        SOP_REPLACE, /**< Replace the stencil value with the reference value. */
        SOP_INCREMENT, /**< Increase the stencil value by 1, clamping at the maximum value. */
        SOP_DECREMENT, /**< Decrease the stencil value by 1, clamping at 0. */
        SOP_INCREMENT_WRAP, /**< Increase the stencil value by 1, wrapping back to 0 when incrementing past the maximum value. */
        SOP_DECREMENT_WRAP, /**< Decrease the stencil value by 1, wrapping when decrementing 0. */
        SOP_INVERT /**< Invert the bits of the stencil buffer. */
    };

    /** Types of programs that may run on GPU. */
	enum GpuProgramType
	{
		GPT_VERTEX_PROGRAM, /**< Vertex program. */
		GPT_FRAGMENT_PROGRAM, /**< Fragment(pixel) program. */
		GPT_GEOMETRY_PROGRAM, /**< Geometry program. */
		GPT_DOMAIN_PROGRAM, /**< Domain (tesselation evaluation) program. */
		GPT_HULL_PROGRAM, /**< Hull (tesselation control) program. */
		GPT_COMPUTE_PROGRAM, /**< Compute program. */
		GPT_COUNT // Keep at end
	};

	/** Describes operation that will be used for rendering a certain set of vertices. */
	enum  DrawOperationType
	{
		/** Each vertex represents a point. */
		DOT_POINT_LIST = 1,
		/** Each sequential pair of vertices represent a line. */
		DOT_LINE_LIST = 2,
		/** Each vertex (except the first) forms a line with the previous vertex. */
		DOT_LINE_STRIP = 3,
		/** Each sequential 3-tuple of vertices represent a triangle. */
		DOT_TRIANGLE_LIST = 4,
		/** Each vertex (except the first two) form a triangle with the previous two vertices. */
		DOT_TRIANGLE_STRIP = 5,
		/** Each vertex (except the first two) form a triangle with the first vertex and previous vertex. */
		DOT_TRIANGLE_FAN = 6
	};

	/**
	 * Controls what kind of mobility restrictions a scene object has. This is used primarily as a performance hint to
	 * other systems. Generally the more restricted the mobility the higher performance can be achieved.
	 */
	enum class ObjectMobility
	{
		/** Scene object can be moved and has no mobility restrictions. */
		Movable,
		/**
		 * Scene object isn't allowed to be moved but is allowed to be visually changed in other ways (e.g. changing the
		 * displayed mesh or light intensity (depends on attached components).
		 */
		Immovable,
		/** Scene object isn't allowed to be moved nor is it allowed to be visually changed. Object must be fully static. */
		Static
	};
}

#pragma once

namespace te
{
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
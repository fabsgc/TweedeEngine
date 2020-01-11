#pragma once

namespace te
{
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
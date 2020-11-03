#pragma once

#include "TeCorePrerequisites.h"
#include "Resources/TeResource.h"
#include "Math/TeBounds.h"
#include "RenderAPI/TeCommonTypes.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "RenderAPI/TeVertexData.h"
#include "RenderAPI/TeSubMesh.h"
#include "TeMeshData.h"

namespace te
{
    /**
     * Planned usage for the mesh. These options usually affect performance and you should specify static if you don't plan
     * on modifying the mesh often, otherwise specify dynamic.
     */
    enum MeshUsage
    {
        /** Specify for a mesh that is not often updated from the CPU. */
        MU_STATIC = 1 << 0,

        /** Specify for a mesh that is often updated from the CPU. */
        MU_DYNAMIC = 1 << 1,
        /**
         * All mesh data will also be cached in CPU memory, making it available for fast read access from the CPU. Can be
         * combined with other usage flags.
         */
        MU_CPUCACHED = 0x1000
    };

    /** Descriptor object used for creation of a new Mesh object. */
    struct TE_CORE_EXPORT MESH_DESC
    {
        MESH_DESC() { }

        /** Number of vertices in the mesh. */
        UINT32 NumVertices = 0;

        /** Number of indices in the mesh. */
        UINT32 NumIndices = 0;

        /**
         * Vertex description structure that describes how are vertices organized in the vertex buffer. When binding a mesh
         * to the pipeline you must ensure vertex description at least partially matches the input description of the
         * currently bound vertex GPU program.
         */
        SPtr<VertexDataDesc> VertexDesc;

        /**
         * Defines how are indices separated into sub-meshes, and how are those sub-meshes rendered. Sub-meshes may be
         * rendered independently.
         */
        Vector<SubMesh> SubMeshes;

        /** Optimizes performance depending on planned usage of the mesh. */
        INT32 Usage = MU_STATIC;

        /**
         * Size of indices, use smaller size for better performance, however be careful not to go over the number of
         * vertices limited by the size.
         */
        IndexType IndType = IT_32BIT;

        /** Optional skeleton that can be used for skeletal animation of the mesh. */
        SPtr<Skeleton> MeshSkeleton;

        static MESH_DESC DEFAULT;
    };

    /** Properties of a Mesh. Shared between sim and core thread versions of a Mesh. */
    class TE_CORE_EXPORT MeshProperties
    {
    public:
        MeshProperties();
        MeshProperties(UINT32 numVertices, UINT32 numIndices, DrawOperationType drawOp);
        MeshProperties(UINT32 numVertices, UINT32 numIndices, const Vector<SubMesh>& subMeshes);

        /**
         * Retrieves a sub-mesh containing data used for rendering a certain portion of this mesh. If no sub-meshes are
         * specified manually a special sub-mesh containing all indices is returned.
         */
        SubMesh& GetSubMesh(UINT32 subMeshIdx = 0);

        /** Retrieves a total number of sub-meshes in this mesh. */
        UINT32 GetNumSubMeshes() const;

        /** Returns maximum number of vertices the mesh may store. */
        UINT32 GetNumVertices() const { return _numVertices; }

        /** Returns maximum number of indices the mesh may store. */
        UINT32 GetNumIndices() const { return _numIndices; }

        /** Returns bounds of the geometry contained in the vertex buffers for all sub-meshes. */
        const Bounds& GetBounds() const { return _bounds; }

    protected:
        friend class Mesh;

        Vector<SubMesh> _subMeshes;
        UINT32 _numVertices;
        UINT32 _numIndices;
        Bounds _bounds;
    };

    class TE_CORE_EXPORT Mesh : public Resource
    {
    public:
        virtual ~Mesh();

        /** @copydoc CoreObject::Initialize */
        void Initialize() override;

        /**  @copydoc Resource::GetResourceType */
        static UINT32 GetResourceType() { return TID_Mesh; }

        /** Get vertex data used for rendering. */
        SPtr<VertexData> GetVertexData() const;

        /** Get index data used for rendering. */
        SPtr<IndexBuffer> GetIndexBuffer() const;

        /** Returns a structure that describes how are the vertices stored in the mesh's vertex buffer. */
        SPtr<VertexDataDesc> GetVertexDesc() const ;

        /**
         * Returns an offset into the vertex buffers that is returned by getVertexData() that signifies where this meshes
         * vertices begin.
         * 
         * @note Used when multiple meshes share the same buffers.
         */
        virtual UINT32 GetVertexOffset() const { return 0; }

        /**
         * Returns an offset into the index buffer that is returned by getIndexData() that signifies where this meshes
         * indices begin.
         * 
         * @note Used when multiple meshes share the same buffers.
         */
        virtual UINT32 GetIndexOffset() const { return 0; }

        /**
         * Allocates a buffer that exactly matches the size of this mesh. This is a helper function, primarily meant for
         * creating buffers when reading from, or writing to a mesh.
         *
         * @note Thread safe.
         */
        SPtr<MeshData> AllocateBuffer() const;

        /**
         * Returns mesh data cached in the system memory. If the mesh wasn't created with CPU cached usage flag this
         * method will not return any data. Caller should not modify the returned data.
         *
         * @note
         * The data read is the cached mesh data. Any data written to the mesh from the GPU or core thread will not be
         * reflected in this data. Use readData() if you require those changes.
         */
        SPtr<MeshData> GetCachedData() const { return _CPUData; }

        /**
         * Called whenever this mesh starts being used on the GPU.
         * 
         * @note Needs to be called after all commands referencing this mesh have been sent to the GPU.
         */
        virtual void _notifyUsedOnGPU() { }

        /** Returns properties that contain information about the mesh. */
        MeshProperties& GetProperties() { return _properties; }
    
        /**
         * Creates a new empty mesh.
         *
         * @param[in]   desc    Descriptor containing the properties of the mesh to create.
         */
        static HMesh Create(const MESH_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /**
         * Creates a new mesh from an existing mesh data. Created mesh will match the vertex and index buffers described
         * by the mesh data exactly. Mesh will have no sub-meshes.
         *
         * @param[in]   initialData Vertex and index data to initialize the mesh with.
         * @param[in]   desc        Descriptor containing the properties of the mesh to create. Vertex and index count,
         *                          vertex descriptor and index type properties are ignored and are read from provided
         *                          mesh data instead.
         */
        static HMesh Create(const SPtr<MeshData>& initialData, const MESH_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /**
         * Creates a new mesh from an existing mesh data. Created mesh will match the vertex and index buffers described
         * by the mesh data exactly. Mesh will have no sub-meshes.
         *
         * @param[in]   initialData Vertex and index data to initialize the mesh with.
         * @param[in]   usage       Optimizes performance depending on planned usage of the mesh.
         * @param[in]   drawOp      Determines how should the provided indices be interpreted by the pipeline. Default
         *                          option is a triangle strip, where three indices represent a single triangle.
         */
        static HMesh Create(const SPtr<MeshData>& initialData, int usage = MU_STATIC,
            DrawOperationType drawOp = DOT_TRIANGLE_LIST, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /**
         * Creates a new empty mesh. Created mesh will have no sub-meshes.
         *
         * @param[in]   numVertices     Number of vertices in the mesh.
         * @param[in]   numIndices      Number of indices in the mesh.
         * @param[in]   vertexDesc      Vertex description structure that describes how are vertices organized in the
         *                              vertex buffer. When binding a mesh to the pipeline you must ensure vertex
         *                              description at least partially matches the input description of the currently bound
         *                              vertex GPU program.
         * @param[in]   usage           Optimizes performance depending on planned usage of the mesh.
         * @param[in]   drawOp          Determines how should the provided indices be interpreted by the pipeline. Default
         *                              option is a triangle list, where three indices represent a single triangle.
         * @param[in]   indexType       Size of indices, use smaller size for better performance, however be careful not to
         *                              go over the number of vertices limited by the size.
         * @param[in]   deviceMask      Mask that determines on which GPU devices should the object be created on.
         */
        static HMesh Create(UINT32 numVertices, UINT32 numIndices, const SPtr<VertexDataDesc>& vertexDesc,
            int usage = MU_STATIC, DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexType indexType = IT_32BIT, 
            GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /**
         * @copydoc Create(const MESH_DESC&)
         *
         * @note    Internal method. Use create() for normal use.
         */
        static SPtr<Mesh> _createPtr(const MESH_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /**
         * @copydoc Create(const SPtr<MeshData>&, const MESH_DESC&)
         *
         * @note    Internal method. Use create() for normal use.
         */
        static SPtr<Mesh> _createPtr(const SPtr<MeshData>& initialData, const MESH_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /**
         * @copydoc Create(const SPtr<MeshData>&, int, DrawOperationType)
         *
         * @note    Internal method. Use create() for normal use.
         */
        static SPtr<Mesh> _createPtr(const SPtr<MeshData>& initialData, int usage = MU_STATIC,
            DrawOperationType drawOp = DOT_TRIANGLE_LIST, GpuDeviceFlags deviceMask = GDF_DEFAULT);

        /**
         * Creates a new empty and uninitialized mesh. You will need to manually initialize the mesh before using it.
         *
         * @note This should only be used for special cases like serialization and is not meant for normal use.
         */
        static SPtr<Mesh> CreateEmpty();

        /**
         * Updates the current mesh with the provided data.
         *
         * @param[in]  data                 Data to update the mesh with.
         * @param[in]  discardEntireBuffer  When true the existing contents of the resource you are updating will be
         *                                  discarded. This can make the operation faster. Resources with certain buffer
         *                                  types might require this flag to be in a specific state otherwise the operation
         *                                  will fail.
         * @param[in]  updateBounds         If true the internal bounds of the mesh will be recalculated based on the
         *                                  provided data.
         * @param[in]  queueIdx             Device queue to perform the write operation on. See @ref queuesDoc.
         */
        virtual void WriteData(const MeshData& data, bool discardEntireBuffer, bool updateBounds = true,
            UINT32 queueIdx = 0);

        /**
         * Reads the current mesh data into the provided @p data parameter. Data buffer needs to be pre-allocated.
         *
         * @param[out] data Pre-allocated buffer of proper vertex/index format and size where data will be
         *                                  read to. You can use Mesh::allocBuffer() to allocate a buffer of a correct
         *                                  format and size.
         * @param[in] deviceIdx             Index of the device whose memory to read. If the buffer doesn't exist on this
         *                                  device, no data will be read.
         * @param[in] queueIdx              Device queue to perform the read operation on. See @ref queuesDoc.
         */
        virtual void ReadData(MeshData& data, UINT32 deviceIdx = 0, UINT32 queueIdx = 0);

    protected:
        friend class MeshManager;

        Mesh(const MESH_DESC& desc, GpuDeviceFlags deviceMask);
        Mesh(const SPtr<MeshData>& initialMeshData, const MESH_DESC& desc, GpuDeviceFlags deviceMask);

    private:
        Mesh();

        /** Updates bounds by calculating them from the vertices in the provided mesh data object. */
        void UpdateBounds(const MeshData& meshData);

        /**
         * Creates buffers used for caching of CPU mesh data.
         *
         * @note Make sure to initialize all mesh properties before calling this.
         */
        void CreateCPUBuffer();

        /** Updates the cached CPU buffers with new data. */
        void UpdateCPUBuffer(UINT32 subresourceIdx, const MeshData& meshData);

    private:
        MeshProperties _properties;

        mutable SPtr<MeshData> _CPUData;

        SPtr<VertexData> _vertexData;
        SPtr<IndexBuffer> _indexBuffer;
        SPtr<VertexDataDesc> _vertexDesc;

        SPtr<MeshData> _tempInitialMeshData;

        int _usage;
        IndexType _indexType;
        GpuDeviceFlags _deviceMask;
    };
}

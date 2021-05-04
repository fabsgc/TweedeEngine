#include "TeGLVertexArrayObjectManager.h"
#include "TeGLVertexBuffer.h"
#include "RenderAPI/TeVertexDeclaration.h"
#include "TeGLGpuProgram.h"
#include "TeGLHardwareBufferManager.h"

#define VBO_BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace te
{
    GLVertexArrayObject::GLVertexArrayObject(GLuint handle, UINT64 vertexProgramId,
        GLVertexBuffer** attachedBuffers, UINT32 numBuffers)
        : _handle(handle)
        , _vertProgId(vertexProgramId)
        , _attachedBuffers(attachedBuffers)
        , _numBuffers(numBuffers)
    { }

    ::std::size_t GLVertexArrayObject::Hash::operator()(const GLVertexArrayObject &vao) const
    {
        std::size_t seed = 0;
        te_hash_combine(seed, vao._vertProgId);

        for (UINT32 i = 0; i < vao._numBuffers; i++)
            te_hash_combine(seed, vao._attachedBuffers[i]->GetGLBufferId());

        return seed;
    }

    bool GLVertexArrayObject::Equal::operator()(const GLVertexArrayObject &a, const GLVertexArrayObject &b) const
    {
        if (a._vertProgId != b._vertProgId)
            return false;

        if (a._numBuffers != b._numBuffers)
            return false;

        for (UINT32 i = 0; i < a._numBuffers; i++)
        {
            if (a._attachedBuffers[i]->GetGLBufferId() != b._attachedBuffers[i]->GetGLBufferId())
                return false;
        }

        return true;
    }

    bool GLVertexArrayObject::operator== (const GLVertexArrayObject& obj)
    {
        if (_vertProgId != obj._vertProgId)
            return false;

        if (_numBuffers != obj._numBuffers)
            return false;

        for (UINT32 i = 0; i < _numBuffers; i++)
        {
            if (_attachedBuffers[i]->GetGLBufferId() != obj._attachedBuffers[i]->GetGLBufferId())
                return false;
        }

        return true;
    }

    bool GLVertexArrayObject::operator!= (const GLVertexArrayObject& obj)
    {
        return !operator==(obj);
    }

    GLVertexArrayObjectManager::~GLVertexArrayObjectManager()
    {
        assert(_VAObjects.size() == 0 && "VertexArrayObjectManager getting shut down but not all VA objects were released.");
    }

    const GLVertexArrayObject& GLVertexArrayObjectManager::GetVAO(const SPtr<GLGpuProgram>& vertexProgram,
        const SPtr<VertexDeclaration>& vertexDecl, const std::array<SPtr<VertexBuffer>, 32>& boundBuffers)
    {
        UINT32 maxStreamIdx = 0;
        const Vector<VertexElement>& decl = vertexDecl->GetProperties().GetElements();
        for (auto& elem : decl)
            maxStreamIdx = std::max(maxStreamIdx, elem.GetStreamIdx());

        UINT32 numStreams = maxStreamIdx + 1;
        UINT32 numUsedBuffers = 0;
        INT32* streamToSeqIdx = te_allocate<INT32>(numStreams * sizeof(INT32));
        GLVertexBuffer** usedBuffers = te_allocate<GLVertexBuffer*>((UINT32)boundBuffers.size() * sizeof(GLVertexBuffer*));
        
        memset(usedBuffers, 0, (UINT32)boundBuffers.size() * sizeof(GLVertexBuffer*));

        for (UINT32 i = 0; i < numStreams; i++)
            streamToSeqIdx[i] = -1;

        for (auto& elem : decl)
        {
            UINT32 streamIdx = elem.GetStreamIdx();
            if (streamIdx >= (UINT32)boundBuffers.size())
                continue;

            if (streamToSeqIdx[streamIdx] != -1) // Already visited
                continue;

            SPtr<VertexBuffer> vertexBuffer = boundBuffers[streamIdx];
            streamToSeqIdx[streamIdx] = (INT32)numUsedBuffers;

            if (vertexBuffer != nullptr)
                usedBuffers[numUsedBuffers] = static_cast<GLVertexBuffer*>(vertexBuffer.get());
            else
                usedBuffers[numUsedBuffers] = nullptr;

            numUsedBuffers++;
        }
        
        GLVertexArrayObject wantedVAO(0, vertexProgram->GetGLHandle(), usedBuffers, numUsedBuffers);

        auto findIter = _VAObjects.find(wantedVAO);
        if (findIter != _VAObjects.end())
        {
            te_free(usedBuffers);
            te_free(streamToSeqIdx);

            return *findIter; // Found existing, return that
        }

        // Need to create new VAO
        const Vector<VertexElement>& inputAttributes = vertexProgram->GetInputDeclaration()->GetProperties().GetElements();

        glGenVertexArrays(1, &wantedVAO._handle);
        TE_CHECK_GL_ERROR();

        glBindVertexArray(wantedVAO._handle);
        TE_CHECK_GL_ERROR();

        for (auto& elem : decl)
        {
            UINT32 streamIdx = elem.GetStreamIdx();
            INT32 seqIdx = streamToSeqIdx[streamIdx];

            if (seqIdx == -1)
                continue;

            bool foundSemantic = false;
            GLint attribLocation = 0;
            for (auto iter = inputAttributes.begin(); iter != inputAttributes.end(); ++iter)
            {
                if (iter->GetSemantic() == elem.GetSemantic() && iter->GetSemanticIdx() == elem.GetSemanticIdx())
                {
                    foundSemantic = true;
                    attribLocation = iter->GetOffset();
                    break;
                }
            }

            if (!foundSemantic) // Shader needs to have a matching input attribute, otherwise we skip it
                continue;

            // TODO - We might also want to check the size of input and buffer, and make sure they match? Or does OpenGL handle that internally?

            GLVertexBuffer* vertexBuffer = usedBuffers[seqIdx];
            const VertexBufferProperties& vbProps = vertexBuffer->GetProperties();

            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->GetGLBufferId());
            TE_CHECK_GL_ERROR();

            void* bufferData = VBO_BUFFER_OFFSET(elem.GetOffset());

            UINT32 typeCount = VertexElement::GetTypeCount(elem.GetType());
            GLenum glType = GLHardwareBufferManager::GetGLType(elem.GetType());
            bool isInteger = glType == GL_SHORT || glType == GL_UNSIGNED_SHORT || glType == GL_INT
                || glType == GL_UNSIGNED_INT || glType == GL_UNSIGNED_BYTE;

            GLboolean normalized = GL_FALSE;
            switch (elem.GetType())
            {
            case VET_COLOR:
            case VET_COLOR_ABGR:
            case VET_COLOR_ARGB:
            case VET_UBYTE4_NORM:
                normalized = GL_TRUE;
                isInteger = false;
                break;
            default:
                break;
            }

            GLsizei vertexSize = static_cast<GLsizei>(vbProps.GetVertexSize());
            if(isInteger)
            {
                glVertexAttribIPointer(attribLocation, typeCount, glType, vertexSize, bufferData);
                TE_CHECK_GL_ERROR();
            }
            else
            {
                glVertexAttribPointer(attribLocation, typeCount, glType, normalized, vertexSize, bufferData);
                TE_CHECK_GL_ERROR();
            }

            glVertexAttribDivisor(attribLocation, elem.GetInstanceStepRate());
            TE_CHECK_GL_ERROR();

            glEnableVertexAttribArray(attribLocation);
            TE_CHECK_GL_ERROR();
        }

        wantedVAO._attachedBuffers = (GLVertexBuffer**)te_allocate(numUsedBuffers * sizeof(GLVertexBuffer*));
        for (UINT32 i = 0; i < numUsedBuffers; i++)
        {
            wantedVAO._attachedBuffers[i] = usedBuffers[i];
            usedBuffers[i]->RegisterVAO(wantedVAO);
        }

        te_free(usedBuffers);
        te_free(streamToSeqIdx);

        auto iter = _VAObjects.insert(wantedVAO);

        return *iter.first;
    }

    // Note: This must receieve a copy and not a ref because original will be destroyed
    void GLVertexArrayObjectManager::NotifyBufferDestroyed(GLVertexArrayObject vao)
    {
        _VAObjects.erase(vao);

        for (UINT32 i = 0; i < vao._numBuffers; i++)
        {
            vao._attachedBuffers[i]->UnregisterVAO(vao);
        }

        glDeleteVertexArrays(1, &vao._handle);
        TE_CHECK_GL_ERROR();

        te_free(vao._attachedBuffers);
    }
}

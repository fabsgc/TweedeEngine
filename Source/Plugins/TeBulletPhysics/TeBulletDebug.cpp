#include "TeBulletDebug.h"
#include "TeBulletDebugMat.h"
#include "RenderAPI/TeRenderTexture.h"
#include "RenderAPI/TeVertexBuffer.h"
#include "RenderAPI/TeVertexDataDesc.h"
#include "RenderAPI/TeVertexDeclaration.h"
#include "Renderer/TeCamera.h"

namespace te
{
    BulletDebug::BulletDebug()
    {
        _debugMode = DBG_DrawWireframe | DBG_DrawContactPoints | DBG_DrawConstraints | DBG_DrawConstraintLimits;

        _material = BulletDebugMat::Get();
        CreateInstanceBuffer(_instanceBuffer);
    }

    BulletDebug::~BulletDebug()
    {
        if (_instanceBuffer.PointVB)
        {
            _instanceBuffer.PointVB->Destroy();
            _instanceBuffer.PointVB = nullptr;
        }
    }

    void BulletDebug::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
    {
        _debugElements.push_back({ from, to, fromColor, toColor });
    }

    void BulletDebug::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
    {
        const btVector3& from = PointOnB;
        const btVector3 to = PointOnB + normalOnB * distance;
        drawLine(from, to, color);
    }

    void BulletDebug::reportErrorWarning(const char* error_warning)
    {
        TE_DEBUG(error_warning);
    }

    void BulletDebug::Draw(const SPtr<Camera>& camera, const SPtr<RenderTarget>& renderTarget)
    {
        RenderAPI& rapi = RenderAPI::Instance();
        UINT32 clearBuffers = FBT_DEPTH;

        Vector<PerBulletDebugInstanceData> instancedElements;

        for (auto& element : _debugElements)
        {
            // TODO
        }

        if (instancedElements.size() > 0)
        {
            rapi.SetRenderTarget(camera->GetViewport()->GetTarget());
            rapi.ClearViewport(clearBuffers, Color::Black);

            _material->BindCamera(camera);

            rapi.SetVertexDeclaration(_instanceBuffer.PointVDecl);
            rapi.SetVertexBuffers(0, &_instanceBuffer.PointVB, 1);
            rapi.SetDrawOperation(DOT_POINT_LIST);
        }
    }

    void BulletDebug::Clear()
    {
        _debugElements.clear();
    }

    void BulletDebug::CreateInstanceBuffer(InstanceBuffer& buffer)
    {
        buffer.PointVDesc = te_shared_ptr_new<VertexDataDesc>();
        buffer.PointVDesc->AddVertElem(VET_FLOAT3, VES_POSITION);

        buffer.PointVDecl = VertexDeclaration::Create(buffer.PointVDesc);

        VERTEX_BUFFER_DESC vbDesc;
        vbDesc.VertexSize = buffer.PointVDecl->GetProperties().GetVertexSize(0);
        vbDesc.NumVerts = 1;
        vbDesc.Usage = GBU_DYNAMIC;

        buffer.PointVB = VertexBuffer::Create(vbDesc);

        buffer.PointData = (VertexBufferLayout*)buffer.PointVB->Lock(
            0, sizeof(VertexBufferLayout), GBL_WRITE_ONLY_NO_OVERWRITE);

        if (buffer.PointData)
            buffer.PointData[0].Position = Vector3(0.0f, 0.0f, 0.0f);

        buffer.PointVB->Unlock();
    }
}

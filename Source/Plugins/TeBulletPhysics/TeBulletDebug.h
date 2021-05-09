#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysicsDebug.h"
#include "Utility/TePoolAllocator.h"
#include "TeBulletDebugMat.h"

namespace te 
{
    class BulletDebugMat;

    class BulletDebug : public PhysicsDebug, public btIDebugDraw
    {
        struct DebugElement
        {
            btVector3 From;
            btVector3 To;
            btVector3 FromColor;
            btVector3 ToColor;
        };

        struct VertexBufferLayout
        {
            Vector3 Position;
        };

        struct InstanceBuffer
        {
            SPtr<VertexBuffer> PointVB;
            SPtr<VertexDataDesc> PointVDesc;
            SPtr<VertexDeclaration> PointVDecl;
            VertexBufferLayout* PointData = nullptr;
        };

    public:
        BulletDebug();
        ~BulletDebug();

        void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;
        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override { drawLine(from, to, color, color); }
        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
        void reportErrorWarning(const char* warningString) override;
        void draw3dText(const btVector3& location, const char* textString) override {}
        void setDebugMode(const int debugMode) override { _debugMode = debugMode; }
        int getDebugMode() const override { return _debugMode; }

        void Draw(const SPtr<Camera>& camera, const SPtr<RenderTarget>& renderTarget) override;
        void Clear() override;

    private:
        void CreateInstanceBuffer(InstanceBuffer& buffer);

    private:
        int _debugMode;
        List<DebugElement> _debugElements;

        BulletDebugMat* _material;
        InstanceBuffer _instanceBuffer;
    };

    IMPLEMENT_GLOBAL_POOL(PerBulletDebugInstanceData, MAX_BULLET_DEBUG_INSTANCED_BLOCK)
}

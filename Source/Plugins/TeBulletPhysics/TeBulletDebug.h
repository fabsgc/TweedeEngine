#pragma once

#include "TeBulletPhysicsPrerequisites.h"
#include "Physics/TePhysicsDebug.h"
#include "LinearMath/btIDebugDraw.h"
#include "LinearMath/btVector3.h"

namespace te 
{
    class BulletDebug : public PhysicsDebug, public btIDebugDraw
    {
        struct DebugElement
        {
            btVector3 From;
            btVector3 To;
            btVector3 FromColor;
            btVector3 toColor;
        };

    public:
        BulletDebug();
        ~BulletDebug() = default;

        void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;
        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override { drawLine(from, to, color, color); }
        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
        void reportErrorWarning(const char* warningString) override;
        void draw3dText(const btVector3& location, const char* textString) override {}
        void setDebugMode(const int debugMode) override { _debugMode = debugMode; }
        int getDebugMode() const override { return _debugMode; }

        void Draw(const SPtr<RenderTarget>& renderTarget) override;
        void Clear() override;

    private:
        int _debugMode;
        Vector<DebugElement> _debugElements;
    };
}

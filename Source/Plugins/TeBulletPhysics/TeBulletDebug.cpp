#include "TeBulletDebug.h"
#include "RenderAPI/TeRenderTexture.h"

namespace te
{
    BulletDebug::BulletDebug()
    {
        _debugMode = DBG_DrawWireframe | DBG_DrawContactPoints | DBG_DrawConstraints | DBG_DrawConstraintLimits;
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

    void BulletDebug::Draw(const SPtr<RenderTarget>& renderTarget)
    {
        for (auto& element : _debugElements)
        {
            // TODO
        }
    }

    void BulletDebug::Clear()
    {
        _debugElements.clear();
    }
}

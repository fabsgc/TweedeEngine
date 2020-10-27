#pragma once

#include "TeCorePrerequisites.h"
#include "TeWidget.h"
#include "../TeEditor.h"
#include "Scene/TeTransform.h"

namespace te
{
    class WidgetProperties : public Widget
    {
    public:
        WidgetProperties();
        ~WidgetProperties();

        virtual void Initialize() override;
        virtual void Update() override;
        virtual void UpdateBackground() override;

    protected:

        /** Shows element properties, returns true if something has been modified */
        bool ShowSceneObjectProperties();

        /**  @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCCameraProperties();

        /**  @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCCameraFlyerProperties();

        /**  @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCCameraUIProperties();

        /**  @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCLightProperties();

        /**  @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCRenderableProperties();

        /**  @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowScriptProperties();

        /**  @copydoc WidgetProperties::ShowSceneObjectProperties */
        bool ShowCSkyboxProperties();

        /**  Show transform and mobility options */
        bool ShowTransform(Transform& transform, ObjectMobility& mobility);

        /**  Show information about a GameOject (both Component and SceneObject) */
        bool ShowGameObjectInformation(SPtr<GameObject>& gameObject);

    protected:
        Editor::SelectionData& _selections;
    };
}

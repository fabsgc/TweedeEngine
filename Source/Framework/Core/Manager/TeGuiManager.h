#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    /** Manager that handles gui render system start up. */
    class TE_CORE_EXPORT GuiManager : public Module<GuiManager>
    {
    public:
        explicit GuiManager(const String& pluginName);
        virtual ~GuiManager();

        TE_MODULE_STATIC_HEADER_MEMBER(GuiManager)

        /** Returns the current GUI API. Null if no GUI API is active. */
        SPtr<GuiAPI> GetGui() { return _gui; }

        /** Registers a factory responsible for creating a specific GUI API type. */
        void RegisterFactory(SPtr<GuiAPIFactory> factory);

    protected:
        /** @copydoc Module::OnStartUp */
        void OnStartUp() override;

        /** @copydoc Module::OnShutDown */
        void OnShutDown() override;

    private:
        String _pluginName;
        DynLib* _pluginLibrary;
        Vector<SPtr<GuiAPIFactory>> _availableFactories;
        SPtr<GuiAPIFactory> _guiFactory;
        SPtr<GuiAPI> _gui;
    };
}

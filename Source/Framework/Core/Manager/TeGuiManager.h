#pragma once

#include "TeCorePrerequisites.h"
#include "Utility/TeModule.h"

namespace te
{
    /** Manager that handles gui render system start up. */
    class TE_CORE_EXPORT GuiManager : public Module<GuiManager>
    {
    public:
        GuiManager();
        virtual ~GuiManager();

        TE_MODULE_STATIC_HEADER_MEMBER(GuiManager)

        /**
         * Starts the gui render API with the provided name
         * @param[in]	name	Name of the render system to start. Factory for this render system must be
         */
        SPtr<GuiAPI> Initialize(const String& name);

         /** Returns the current renderer. Null if no renderer is active. */
        SPtr<GuiAPI> GetGui() { return _gui; }

        /**	Registers a new render API factory responsible for creating a specific render system type. */
        void RegisterFactory(SPtr<GuiAPIFactory> factory);

    private:
        Vector<SPtr<GuiAPIFactory>> _availableFactories;
        bool _guiInitialized;
        SPtr<GuiAPI> _gui;
    };
}

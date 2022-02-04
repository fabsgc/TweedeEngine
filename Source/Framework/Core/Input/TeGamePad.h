#pragma once

#include "TeCorePrerequisites.h"

namespace te
{
    struct GamePadInfo;

    /** Represents a single hardware gamepad. Used by the Input to report gamepad input events. */
    class TE_CORE_EXPORT GamePad
    {
    public:
        struct Pimpl;

        GamePad(const String& name, const GamePadInfo& gamepadInfo, Input* owner);
        virtual ~GamePad();

        /** Returns the name of the device. */
        const String& getName() const { return _name; }

        /** Captures the input since the last call and triggers the events on the parent Input. */
        void Capture();

    public:
        /** Minimum allowed value as reported by the axis movement events. */
        static constexpr int MIN_AXIS = -32768;

        /** Maximum allowed value as reported by the axis movement events. */
        static constexpr int MAX_AXIS = 32767;

    private:
        /** Changes the capture context. Should be called when focus is moved to a new window. */
        void ChangeCaptureContext(UINT64 windowHandle);

    private:
        friend class Input;

        String _name;
        Input* _owner;

        Pimpl* _data;
    };
}
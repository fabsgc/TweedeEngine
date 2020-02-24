#include "Input/TeInputConfiguration.h"
#include "Input/TeInputData.h"

namespace te
{
    UINT32 VirtualButton::NextButtonId = 0;

    Map<String, UINT32> VirtualAxis::UniqueAxisIds;
    UINT32 VirtualAxis::NextAxisId = 0;

    VIRTUAL_BUTTON_DESC::VIRTUAL_BUTTON_DESC(ButtonCode buttonCode, ButtonModifier modifiers, bool repeatable)
        :BtnCode(buttonCode), Modifiers(modifiers), Repeatable(repeatable)
    { }

    VIRTUAL_AXIS_DESC::VIRTUAL_AXIS_DESC(UINT32 type)
        : Type(type)
    { }

    VirtualButton::VirtualButton(const String& name)
    {
        Map<String, UINT32>& uniqueButtonIds = GetUniqueButtonIds();

        auto findIter = uniqueButtonIds.find(name);

        if (findIter != uniqueButtonIds.end())
            ButtonIdentifier = findIter->second;
        else
        {
            ButtonIdentifier = NextButtonId;
            uniqueButtonIds[name] = NextButtonId++;
        }
    }

    Map<String, UINT32>& VirtualButton::GetUniqueButtonIds()
    {
        static Map<String, UINT32> uniqueButtonIds;
        return uniqueButtonIds;
    }

    VirtualAxis::VirtualAxis(const String& name)
    {
        auto findIter = UniqueAxisIds.find(name);

        if (findIter != UniqueAxisIds.end())
            AxisIdentifier = findIter->second;
        else
        {
            AxisIdentifier = NextAxisId;
            UniqueAxisIds[name] = NextAxisId++;
        }
    }

    void InputConfiguration::RegisterButton(const String& name, ButtonCode buttonCode, ButtonModifier modifiers, bool repeatable)
    {
        Vector<VirtualButtonData>& btnData = _buttons[buttonCode & 0x0000FFFF];

        INT32 idx = -1;
        for (UINT32 i = 0; i < (UINT32)btnData.size(); i++)
        {
            if (btnData[i].Name == name)
            {
                idx = (INT32)i;
                break;
            }
        }

        if (idx == -1)
        {
            idx = (INT32)btnData.size();
            btnData.push_back(VirtualButtonData());
        }

        VirtualButtonData& btn = btnData[idx];
        btn.Name = name;
        btn.Desc = VIRTUAL_BUTTON_DESC(buttonCode, modifiers, repeatable);
        btn.Button = VirtualButton(name);
    }

    void InputConfiguration::UnregisterButton(const String& name)
    {
        Vector<UINT32> toRemove;

        for (UINT32 i = 0; i < TE_Count; i++)
        {
            for (UINT32 j = 0; j < (UINT32)_buttons[i].size(); j++)
            {
                if (_buttons[i][j].Name == name)
                    toRemove.push_back(j);
            }

            UINT32 numRemoved = 0;
            for (auto& toRemoveIdx : toRemove)
            {
                _buttons[i].erase(_buttons[i].begin() + toRemoveIdx - numRemoved);

                numRemoved++;
            }

            toRemove.clear();
        }
    }

    void InputConfiguration::RegisterAxis(const String& name, const VIRTUAL_AXIS_DESC& desc)
    {
        VirtualAxis axis(name);

        if (axis.AxisIdentifier >= (UINT32)_axes.size())
            _axes.resize(axis.AxisIdentifier + 1);

        _axes[axis.AxisIdentifier].Name = name;
        _axes[axis.AxisIdentifier].Desc = desc;
        _axes[axis.AxisIdentifier].Axis = axis;
    }

    void InputConfiguration::UnregisterAxis(const String& name)
    {
        for (UINT32 i = 0; i < (UINT32)_axes.size(); i++)
        {
            if (_axes[i].Name == name)
            {
                _axes.erase(_axes.begin() + i);
                i--;
            }
        }
    }

    bool InputConfiguration::GetButtons(ButtonCode code, UINT32 modifiers, Vector<VirtualButton>& btns, Vector<VIRTUAL_BUTTON_DESC>& btnDesc) const
    {
        const Vector<VirtualButtonData>& btnData = _buttons[code & 0x0000FFFF];

        bool foundAny = false;
        for (UINT32 i = 0; i < (UINT32)btnData.size(); i++)
        {
            if ((((UINT32)btnData[i].Desc.Modifiers) & modifiers) == ((UINT32)btnData[i].Desc.Modifiers))
            {
                btns.push_back(btnData[i].Button);
                btnDesc.push_back(btnData[i].Desc);
                foundAny = true;
            }
        }

        return foundAny;
    }

    bool InputConfiguration::GetAxis(const VirtualAxis& axis, VIRTUAL_AXIS_DESC& axisDesc) const
    {
        if (axis.AxisIdentifier >= (UINT32)_axes.size())
            return false;

        axisDesc = _axes[axis.AxisIdentifier].Desc;
        return true;
    }
}
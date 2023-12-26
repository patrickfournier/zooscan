#pragma once

#include "DeviceOptionState.h"

namespace ZooScan
{
    template<typename TValueType>
    class ChangeOptionCommand : public Zoo::Command
    {
    public:
        uint32_t SettingIndex;
        uint32_t ValueIndex;
        TValueType Value;

        ChangeOptionCommand(uint32_t settingIndex, uint32_t valueIndex, TValueType value)
                : SettingIndex(settingIndex), ValueIndex(valueIndex), Value(value)
        {
        }

        static void Execute(const ChangeOptionCommand<bool>& command, DeviceOptionState *state)
        {
            auto updater = DeviceOptionState::Updater(state);
            updater.SetOption(command.SettingIndex, command.ValueIndex, command.Value);
        }

        static void Execute(const ChangeOptionCommand<int>& command, DeviceOptionState *state)
        {
            auto updater = DeviceOptionState::Updater(state);
            updater.SetOption(command.SettingIndex, command.ValueIndex, command.Value);
        }

        static void Execute(const ChangeOptionCommand<std::string>& command, DeviceOptionState *state)
        {
            auto updater = DeviceOptionState::Updater(state);
            updater.SetOption(command.SettingIndex, command.ValueIndex, command.Value);
        }
    };
}

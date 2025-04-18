#pragma once

#include <cstring>
#include <nlohmann/json.hpp>
#include <utility>

#include "DeviceOptionValue.hpp"
#include "DeviceOptionValueBase.hpp"
#include "DeviceSelectorState.hpp"
#include "Rect.hpp"
#include "ZooLib/ChangesetBase.hpp"
#include "ZooLib/ChangesetManager.hpp"
#include "ZooLib/StateComponent.hpp"

namespace ZooScan
{
    class SaneDevice;

    enum class ScanAreaUnit
    {
        Pixels,
        Millimeters
    };

    struct WidgetIndex
    {
        const uint32_t m_OptionIndex{};
        const uint32_t m_ValueIndex{};

        WidgetIndex() = default;
        WidgetIndex(uint32_t optionIndex, uint32_t valueIndex)
            : m_OptionIndex(optionIndex)
            , m_ValueIndex(valueIndex)
        {
        }
        explicit WidgetIndex(uint64_t hash)
            : m_OptionIndex(static_cast<uint32_t>(hash >> 32))
            , m_ValueIndex(static_cast<uint32_t>(hash))
        {
        }
        WidgetIndex(const WidgetIndex &other) = default;
        WidgetIndex(WidgetIndex &&other) noexcept = default;

        bool operator==(const WidgetIndex &other) const
        {
            return m_OptionIndex == other.m_OptionIndex && m_ValueIndex == other.m_ValueIndex;
        }

        [[nodiscard]] uint64_t Hash() const
        {
            return (static_cast<uint64_t>(m_OptionIndex) << 32) | m_ValueIndex;
        }
    };

    class Changeset : public ZooLib::ChangesetBase
    {
        std::vector<WidgetIndex> m_ChangedIndices;
        bool m_ReloadOptions{};

    public:
        explicit Changeset(uint64_t stateInitialVersion)
            : ChangesetBase(stateInitialVersion)
        {
        }

        void AddChangedIndex(WidgetIndex index)
        {
            m_ChangedIndices.push_back(index);
        }

        [[nodiscard]] const std::vector<WidgetIndex> &ChangedIndices() const
        {
            return m_ChangedIndices;
        }

        void SetReloadOptions(bool reloadOptions)
        {
            m_ReloadOptions = reloadOptions;
        }

        [[nodiscard]] bool RebuildAll() const
        {
            return m_ReloadOptions;
        }

        void Aggregate(const Changeset &changeset)
        {
            ChangesetBase::Aggregate(changeset);

            m_ReloadOptions |= changeset.m_ReloadOptions;

            for (auto changedIndex: changeset.m_ChangedIndices)
            {
                if (std::ranges::find(m_ChangedIndices, changedIndex) == m_ChangedIndices.end())
                {
                    m_ChangedIndices.push_back(changedIndex);
                }
            }
        }
    };

    class DeviceOptionsState : public ZooLib::StateComponent, public ZooLib::ChangesetManager<Changeset>
    {
    public:
        static constexpr uint32_t k_InvalidIndex = std::numeric_limits<uint32_t>::max();

    private:
        const std::string m_DeviceName;
        std::vector<DeviceOptionValueBase *> m_OptionValues;

        uint32_t m_PreviewIndex;
        uint32_t m_ModeIndex;
        uint32_t m_TLXIndex;
        uint32_t m_TLYIndex;
        uint32_t m_BRXIndex;
        uint32_t m_BRYIndex;
        uint32_t m_ResolutionIndex;
        uint32_t m_XResolutionIndex;
        uint32_t m_YResolutionIndex;
        uint32_t m_BitDepthIndex;

        [[nodiscard]] Changeset *GetCurrentChangeset()
        {
            return ChangesetManager::GetCurrentChangeset(Version());
        }

        [[nodiscard]] SaneDevice *GetDevice() const
        {
            if (m_DeviceName.empty() || m_State == nullptr)
            {
                return nullptr;
            }

            const auto deviceSelectorState = m_State->GetStateComponentByType<DeviceSelectorState>();
            if (deviceSelectorState == nullptr)
            {
                return nullptr;
            }
            return deviceSelectorState->GetDeviceByName(m_DeviceName);
        }

        void BuildOptions();

        void AddOptionValue(uint32_t index, DeviceOptionValueBase *optionValue)
        {
            if (m_OptionValues.size() <= index)
            {
                m_OptionValues.resize(index + 1);
            }
            m_OptionValues[index] = optionValue;

            /* Save special indices */

            if (strcmp(optionValue->GetName(), "preview") == 0)
            {
                m_PreviewIndex = index;
            }
            else if (strcmp(optionValue->GetName(), "mode") == 0) // not a "well-known" option
            {
                m_ModeIndex = index;
            }
            else if (strcmp(optionValue->GetName(), "tl-x") == 0)
            {
                m_TLXIndex = index;
            }
            else if (strcmp(optionValue->GetName(), "tl-y") == 0)
            {
                m_TLYIndex = index;
            }
            else if (strcmp(optionValue->GetName(), "br-x") == 0)
            {
                m_BRXIndex = index;
            }
            else if (strcmp(optionValue->GetName(), "br-y") == 0)
            {
                m_BRYIndex = index;
            }
            else if (strcmp(optionValue->GetName(), "resolution") == 0)
            {
                m_ResolutionIndex = index;
            }
            else if (strcmp(optionValue->GetName(), "x-resolution") == 0) // not a "well-known" option
            {
                m_XResolutionIndex = index;
            }
            else if (strcmp(optionValue->GetName(), "y-resolution") == 0) // not a "well-known" option
            {
                m_YResolutionIndex = index;
            }
            else if (strcmp(optionValue->GetName(), "depth") == 0) // not a "well-known" option
            {
                m_BitDepthIndex = index;
            }
        }

        void Clear()
        {
            for (auto optionValue: m_OptionValues)
            {
                delete optionValue;
            }
            m_OptionValues.clear();
            GetCurrentChangeset()->SetReloadOptions(true);
        }


    public:
        DeviceOptionsState(ZooLib::State *state, std::string deviceName)
            : StateComponent(state)
            , m_DeviceName(std::move(deviceName))
            , m_PreviewIndex(k_InvalidIndex)
            , m_ModeIndex(k_InvalidIndex)
            , m_TLXIndex(k_InvalidIndex)
            , m_TLYIndex(k_InvalidIndex)
            , m_BRXIndex(k_InvalidIndex)
            , m_BRYIndex(k_InvalidIndex)
            , m_ResolutionIndex(k_InvalidIndex)
            , m_XResolutionIndex(k_InvalidIndex)
            , m_YResolutionIndex(k_InvalidIndex)
            , m_BitDepthIndex(k_InvalidIndex)
        {
            BuildOptions();
        }

        ~DeviceOptionsState() override
        {
            for (auto optionValue: m_OptionValues)
            {
                delete optionValue;
            }
        }

        class Updater : public StateComponent::Updater<DeviceOptionsState>
        {
        public:
            explicit Updater(DeviceOptionsState *state)
                : StateComponent::Updater<DeviceOptionsState>(state)
            {
            }

            ~Updater() override
            {
                m_StateComponent->PushCurrentChangeset();
            }

            void BuildOptions() const
            {
                m_StateComponent->BuildOptions();
            }

            void SetOptionValue(uint32_t optionIndex, uint32_t valueIndex, bool requestedValue) const;
            void SetOptionValue(uint32_t optionIndex, uint32_t valueIndex, double requestedValue) const;
            void SetOptionValue(uint32_t optionIndex, uint32_t valueIndex, int requestedValue) const;
            void SetOptionValue(uint32_t optionIndex, uint32_t valueIndex, const std::string &requestedValue) const;

            void DeserializeAndApply(const nlohmann::json &json) const;
        };

        [[nodiscard]] const char *GetDeviceModel() const
        {
            auto saneDevice = GetDevice();
            if (saneDevice == nullptr)
            {
                return nullptr;
            }

            return saneDevice->GetModel();
        }

        [[nodiscard]] const char *GetDeviceMaker() const
        {
            auto saneDevice = GetDevice();
            if (saneDevice == nullptr)
            {
                return nullptr;
            }

            return saneDevice->GetVendor();
        }

        [[nodiscard]] size_t GetOptionCount() const
        {
            return m_OptionValues.size();
        }

        [[nodiscard]] const DeviceOptionValueBase *GetOption(uint32_t settingIndex) const
        {
            return m_OptionValues[settingIndex];
        }

        template<typename T>
        [[nodiscard]] const DeviceOptionValue<T> *GetOption(uint32_t settingIndex) const
        {
            return dynamic_cast<const DeviceOptionValue<T> *>(m_OptionValues[settingIndex]);
        }

        [[nodiscard]] bool IsPreview() const;

        [[nodiscard]] std::string GetMode() const;

        [[nodiscard]] ScanAreaUnit GetScanAreaUnit() const;

        [[nodiscard]] Rect<double> GetScanArea() const;

        [[nodiscard]] Rect<double> GetMaxScanArea() const;

        [[nodiscard]] int GetResolution() const;

        [[nodiscard]] int GetXResolution() const;

        [[nodiscard]] int GetYResolution() const;

        [[nodiscard]] int GetBitDepth() const;

        [[nodiscard]] uint32_t PreviewIndex() const
        {
            return m_PreviewIndex;
        }

        [[nodiscard]] uint32_t ModeIndex() const
        {
            return m_ModeIndex;
        }

        [[nodiscard]] uint32_t TLXIndex() const
        {
            return m_TLXIndex;
        }

        [[nodiscard]] uint32_t TLYIndex() const
        {
            return m_TLYIndex;
        }

        [[nodiscard]] uint32_t BRXIndex() const
        {
            return m_BRXIndex;
        }

        [[nodiscard]] uint32_t BRYIndex() const
        {
            return m_BRYIndex;
        }

        [[nodiscard]] uint32_t ResolutionIndex() const
        {
            return m_ResolutionIndex;
        }

        [[nodiscard]] uint32_t XResolutionIndex() const
        {
            return m_XResolutionIndex;
        }

        [[nodiscard]] uint32_t YResolutionIndex() const
        {
            return m_YResolutionIndex;
        }

        [[nodiscard]] uint32_t BitDepthIndex() const
        {
            return m_BitDepthIndex;
        }

        [[nodiscard]] nlohmann::json *Serialize() const;
    };
}

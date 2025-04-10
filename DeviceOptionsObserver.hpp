#pragma once

#include "DeviceOptionsState.hpp"
#include "PreviewState.hpp"
#include "ZooLib/Observer.hpp"

namespace ZooScan
{
    class DeviceOptionsObserver final : public ZooLib::Observer
    {
    protected:
        void UpdateImplementation() override
        {
            const auto deviceOptionsState = dynamic_cast<const DeviceOptionsState *>(m_ObservedComponents[0]);
            auto appState = dynamic_cast<PreviewState *>(m_ModifiedComponents[0]);
            auto updater = PreviewState::Updater(appState);
            updater.UpdatePreviewRectangle(deviceOptionsState->GetScanArea());
        }

    public:
        DeviceOptionsObserver(const DeviceOptionsState *observedStateComponent, PreviewState *modifiedStateComponent)
            : Observer(
                      std::vector<const ZooLib::StateComponent *>({observedStateComponent}),
                      std::vector<ZooLib::StateComponent *>({modifiedStateComponent}))
        {
        }
    };
}

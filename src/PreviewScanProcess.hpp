#pragma once

#include "ScanProcess.hpp"

namespace Gorfector
{
    class PreviewScanProcess final : public ScanProcess
    {
        static constexpr int k_DefaultResolution = 300;

        void SetPreviewOptions() const;
        void RestoreOptionsAfterPreview() const;

    protected:
        bool Update() override
        {
            if (m_PreviewState == nullptr)
            {
                return false;
            }

            return ScanProcess::Update();
        }

        void Stop(bool canceled) override
        {
            ScanProcess::Stop(canceled);

            RestoreOptionsAfterPreview();

            auto updater = AppState::Updater(m_AppState);
            updater.SetIsPreviewing(false);
        }

        bool AfterStartScanChecks() override
        {
            if (m_ScanParameters.format != SANE_FRAME_GRAY && m_ScanParameters.format != SANE_FRAME_RGB)
            {
                ZooLib::ShowUserError(ADW_APPLICATION_WINDOW(m_MainWindow), _("Unsupported format."));
                return false;
            }

            return true;
        }

        void GetBuffer(SANE_Byte *&outBuffer, size_t &outMaxReadLength) override
        {
            auto previewPanelUpdater = PreviewState::Updater(m_PreviewState);
            previewPanelUpdater.GetReadBuffer(outBuffer, outMaxReadLength);
        }

        void CommitBuffer(size_t readLength) override
        {
            auto previewPanelUpdater = PreviewState::Updater(m_PreviewState);
            previewPanelUpdater.CommitReadBuffer(readLength);

            ScanProcess::CommitBuffer(readLength);
        }

    public:
        PreviewScanProcess(
                SaneDevice *device, PreviewState *previewState, AppState *appState, DeviceOptionsState *scanOptions,
                OutputOptionsState *outputOptions, GtkWidget *mainWindow, const std::function<void()> *finishCallback)
            : ScanProcess(device, previewState, appState, scanOptions, outputOptions, mainWindow, finishCallback)
        {
        }

        bool Start() override
        {
            auto updater = AppState::Updater(m_AppState);
            updater.SetIsPreviewing(true);

            SetPreviewOptions();

            // Get actual resolution.
            double previewResolution = k_DefaultResolution;
            auto resolutionDescription = m_Device->GetOptionDescriptor(m_ScanOptions->GetResolutionIndex());
            SANE_Int resolution;
            if (resolutionDescription != nullptr &&
                m_Device->GetOptionValue(m_ScanOptions->GetResolutionIndex(), &resolution))
            {
                previewResolution =
                        resolutionDescription->type == SANE_TYPE_FIXED ? SANE_UNFIX(resolution) : resolution;
            }

            if (!ScanProcess::Start())
            {
                // Stop() has already been called
                updater.SetIsPreviewing(false);
                return false;
            }

            if (m_PreviewState != nullptr)
            {
                auto previewPanelUpdater = PreviewState::Updater(m_PreviewState);
                previewPanelUpdater.PrepareForScan(
                        m_ScanParameters.pixels_per_line, m_ScanParameters.bytes_per_line, m_ScanParameters.lines,
                        m_ScanParameters.depth, m_ScanParameters.format, previewResolution);
            }

            return true;
        }
    };

}

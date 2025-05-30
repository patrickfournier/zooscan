#include <adwaita.h>

#include "PresetPanel.hpp"
#include "PresetPanelDialogs.hpp"
#include "ZooLib/GtkUtils.hpp"
#include "ZooLib/StringUtils.hpp"

namespace Gorfector
{
    static void OnPresetNameUpdated(GtkEntryBuffer *entryBuffer, GParamSpec *paramSpec, gpointer userData)
    {
        auto dialog = ADW_DIALOG(userData);
        auto presetPanel = static_cast<PresetPanel *>(g_object_get_data(G_OBJECT(dialog), "PresetPanel"));
        auto text = gtk_entry_buffer_get_text(entryBuffer);
        auto entryHintLabel = ZooLib::FindWidgetByName(GTK_WIDGET(dialog), "preset-name-hint");
        auto createButton = ZooLib::FindWidgetByName(GTK_WIDGET(dialog), "apply-button");
        auto rowId = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(createButton), "RowId"));
        auto presetToRename = presetPanel->GetPreset(rowId);

        std::string presetName(text);
        ZooLib::Trim(presetName);

        if (presetName.empty())
        {
            gtk_label_set_text(GTK_LABEL(entryHintLabel), _(s_EmptyNameHint));
            gtk_widget_set_sensitive(createButton, false);
        }
        else if (auto existingPreset = presetPanel->GetState()->GetPreset(presetName);
                 existingPreset != presetToRename && existingPreset != nullptr)
        {
            gtk_label_set_text(GTK_LABEL(entryHintLabel), _(s_NameConflictHint));
            gtk_widget_set_sensitive(createButton, false);
        }
        else
        {
            gtk_label_set_text(GTK_LABEL(entryHintLabel), "");
            gtk_widget_set_sensitive(createButton, true);
        }
    }

    void OnUpdatePreset(GtkWidget *widget, gpointer userData)
    {
        auto presetPanel = static_cast<PresetPanel *>(userData);
        if (presetPanel == nullptr)
        {
            return;
        }

        auto dialog = ZooLib::GetParentOfType(GTK_WIDGET(widget), ADW_TYPE_DIALOG);
        if (dialog == nullptr)
        {
            g_warn_message(App::k_ApplicationName, __FILE__, __LINE__, "", "Dialog not found");
            return;
        }

        auto entry = ZooLib::FindWidgetByName(dialog, "preset-name");
        if (entry == nullptr)
        {
            g_warn_message(App::k_ApplicationName, __FILE__, __LINE__, "", "Preset name entry not found");
            return;
        }

        auto entryBuffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
        auto newPresetName = gtk_entry_buffer_get_text(GTK_ENTRY_BUFFER(entryBuffer));

        bool updateValues = false;
        auto updateCheckbox = ZooLib::FindWidgetByName(dialog, "update-values");
        if (updateCheckbox != nullptr && GTK_IS_CHECK_BUTTON(updateCheckbox))
        {
            updateValues = gtk_check_button_get_active(GTK_CHECK_BUTTON(updateCheckbox));
        }

        auto rowId = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "RowId"));

        presetPanel->UpdatePreset(rowId, newPresetName, updateValues);

        adw_dialog_close(ADW_DIALOG(dialog));
    }

    void ShowUpdatePresetDialog(GtkWidget *widget, gpointer userData)
    {
        auto listBoxRow = ZooLib::GetParentOfType(widget, GTK_TYPE_LIST_BOX_ROW);
        auto rowId = gtk_list_box_row_get_index(GTK_LIST_BOX_ROW(listBoxRow));

        auto presetPanel = static_cast<PresetPanel *>(userData);
        if (presetPanel == nullptr)
        {
            return;
        }

        auto preset = presetPanel->GetPreset(rowId);

        auto dialog = adw_dialog_new();
        g_object_set_data(G_OBJECT(dialog), "PresetPanel", userData);
        adw_dialog_set_title(dialog, _("Update Preset"));

        auto box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        adw_dialog_set_child(dialog, box);

        auto header = adw_header_bar_new();
        gtk_box_append(GTK_BOX(box), header);

        auto hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_set_margin_bottom(hbox1, 10);
        gtk_widget_set_margin_top(hbox1, 10);
        gtk_widget_set_margin_start(hbox1, 10);
        gtk_widget_set_margin_end(hbox1, 10);
        gtk_box_append(GTK_BOX(box), hbox1);

        auto label = gtk_label_new(_("Enter a new name for the preset:"));
        gtk_widget_set_margin_end(label, 10);
        gtk_box_append(GTK_BOX(hbox1), label);

        auto entry = gtk_entry_new();
        gtk_widget_set_name(entry, "preset-name");
        gtk_box_append(GTK_BOX(hbox1), entry);
        auto entryBuffer = gtk_entry_get_buffer(GTK_ENTRY(entry));
        g_signal_connect(entryBuffer, "notify::text", G_CALLBACK(OnPresetNameUpdated), dialog);

        auto updateValuesCheckbox = gtk_check_button_new_with_label(_("Update preset values"));
        gtk_widget_set_name(updateValuesCheckbox, "update-values");
        gtk_widget_set_margin_bottom(updateValuesCheckbox, 10);
        gtk_widget_set_margin_start(updateValuesCheckbox, 10);
        gtk_widget_set_margin_end(updateValuesCheckbox, 10);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(updateValuesCheckbox), false);
        gtk_box_append(GTK_BOX(box), updateValuesCheckbox);

        auto bottomBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_widget_set_margin_bottom(bottomBox, 10);
        gtk_widget_set_margin_start(bottomBox, 10);
        gtk_widget_set_margin_end(bottomBox, 10);
        gtk_box_append(GTK_BOX(box), bottomBox);

        auto entryHint = gtk_label_new("");
        gtk_widget_set_name(entryHint, "preset-name-hint");
        gtk_widget_set_css_classes(entryHint, s_EntryHintClasses);
        gtk_widget_set_halign(entryHint, GTK_ALIGN_START);
        gtk_widget_set_hexpand(entryHint, true);
        gtk_widget_set_margin_end(entryHint, 10);
        gtk_box_append(GTK_BOX(bottomBox), entryHint);

        auto createButton = gtk_button_new_with_label(_("Update"));
        gtk_widget_set_name(createButton, "apply-button");
        gtk_widget_set_css_classes(createButton, s_ButtonClasses);
        gtk_widget_set_halign(createButton, GTK_ALIGN_END);
        gtk_widget_set_sensitive(createButton, false);
        gtk_box_append(GTK_BOX(bottomBox), createButton);
        g_object_set_data(G_OBJECT(createButton), "RowId", GINT_TO_POINTER(rowId));
        g_signal_connect(createButton, "clicked", G_CALLBACK(OnUpdatePreset), presetPanel);

        adw_dialog_present(dialog, GTK_WIDGET(presetPanel->GetMainWindow()));

        // Set this late to avoid the signal being triggered before the initial text is set
        gtk_entry_buffer_set_text(
                entryBuffer, (*preset)[PresetPanelState::k_PresetNameKey].get<std::string>().c_str(), -1);
    }
}

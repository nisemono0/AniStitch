#pragma once

#include "ui/ui_stitchersettingsdialog.h"

#include "utils/metatypes.hpp"

#include <QDialog>
#include <QWidget>


namespace Ui {
    class StitcherSettingsDialog;
};

class StitcherSettingsDialog : public QDialog {
    Q_OBJECT;
public:
    explicit StitcherSettingsDialog(QWidget *parent = nullptr);
    ~StitcherSettingsDialog() override;

private:
    // Enums used internally for comboboxes
    enum WaveCorrectData {
        HORIZONTAL = Qt::UserRole + 1,
        VERTICAL   = Qt::UserRole + 2,
        AUTO       = Qt::UserRole + 3
    };
    enum WarperData {
        PLANE       = Qt::UserRole + 1,
        SPHERICAL   = Qt::UserRole + 2,
        CYLINDRICAL = Qt::UserRole + 3,
    };
    enum CompensatorData {
        BLOCKS_GAIN    = Qt::UserRole + 1,
        BLOCKS_CHANNEL = Qt::UserRole + 2,
    };

    Ui::StitcherSettingsDialog *ui;
    // Return a StitcherSettings struct with values from the dialog
    StitcherSettings getStitcherSettings();

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

signals:
    // Send button presses with settings
    void send_StitcherSettingsDialog_scan_settings(const StitcherSettings &stitcher_settings);
    void send_StitcherSettingsDialog_panorama_settings(const StitcherSettings &stitcher_settings);

public slots:
    void receive_show_StitcherSettingsDialog_request();

private slots:
    // Buttons clicked
    void pushButtonStitchScan_clicked(bool checked = false);
    void pushButtonStitchPanorama_clicked(bool checked = false);
    // Reset the option values to default
    void resetValues();

};


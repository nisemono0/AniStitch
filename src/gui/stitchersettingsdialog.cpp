#include "gui/stitchersettingsdialog.hpp"

#include "utils/defs.hpp"


StitcherSettingsDialog::StitcherSettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::StitcherSettingsDialog) {
    this->ui->setupUi(this);

    connect(this->ui->pushButtonStitchScan, &QPushButton::clicked, this, &StitcherSettingsDialog::pushButtonStitchScan_clicked);
    connect(this->ui->pushButtonStitchPanorama, &QPushButton::clicked, this, &StitcherSettingsDialog::pushButtonStitchPanorama_clicked);
    connect(this->ui->pushButtonCancel, &QPushButton::clicked, this, &QDialog::close);
    connect(this->ui->pushButtonResetValues, &QPushButton::clicked, this, &StitcherSettingsDialog::resetValues);
}

StitcherSettingsDialog::~StitcherSettingsDialog() {
    delete this->ui;
}

StitcherSettings StitcherSettingsDialog::getStitcherSettings() {
    StitcherSettings stitcher_settings;

    stitcher_settings.blender_bands = this->ui->spinBoxNumberBands->value();

    // 0 = Horizontal; 1 = Vertical; 2 = Auto
    // I can't be arsed to do this better
    int wave_correct = this->ui->comboBoxWaveCorrect->currentIndex();
    if (wave_correct == 0) {
        stitcher_settings.wave_correct = StitcherSettings::WaveCorrect::HORIZONTAL;
    } else if (wave_correct == 1) {
        stitcher_settings.wave_correct = StitcherSettings::WaveCorrect::VERTICAL;
    } else if (wave_correct == 2) {
        stitcher_settings.wave_correct = StitcherSettings::WaveCorrect::AUTO;
    }

    // 0 = Plane; 1 = Spherical; 2 = Cylindrical
    // Again, I can't be arsed
    int warper = this->ui->comboBoxWarper->currentIndex();
    if (warper == 0) {
        stitcher_settings.warper = StitcherSettings::Warper::PLANE;
    } else if (warper == 1) {
        stitcher_settings.warper = StitcherSettings::Warper::SPHERICAL;
    } else if (warper == 2) {
        stitcher_settings.warper = StitcherSettings::Warper::CYLINDRICAL;
    }

    // 0 = Blocks Gain; 1 = Blocks Channel
    // Really, I can't be arsed
    int exposure_compensator = this->ui->comboBoxExposureCompensator->currentIndex();
    if (exposure_compensator == 0) {
        stitcher_settings.exposure_compensator = StitcherSettings::ExposureCompensator::BLOCKS_GAIN;
    } else if (exposure_compensator == 1) {
        stitcher_settings.exposure_compensator = StitcherSettings::ExposureCompensator::BLOCKS_CHANNEL;
    }

    stitcher_settings.compensator_block_size = this->ui->spinBoxCompensatorBlockSize->value();
    stitcher_settings.compensator_feeds = this->ui->spinBoxCompensatorFeeds->value();

    return stitcher_settings;
}

void StitcherSettingsDialog::receive_show_StitcherSettingsDialog_request() {
    this->resetValues();
    this->show();
}

void StitcherSettingsDialog::pushButtonStitchScan_clicked(bool checked) {
    StitcherSettings stitcher_settings = this->getStitcherSettings();
    emit send_StitcherSettingsDialog_scan_settings(stitcher_settings);

    this->close();
}

void StitcherSettingsDialog::pushButtonStitchPanorama_clicked(bool checked) {
    StitcherSettings stitcher_settings = this->getStitcherSettings();
    emit send_StitcherSettingsDialog_panorama_settings(stitcher_settings);

    this->close();
}

void StitcherSettingsDialog::resetValues() {
    this->ui->spinBoxNumberBands->setValue(0);
    this->ui->comboBoxWaveCorrect->setCurrentIndex(0);
    this->ui->comboBoxWarper->setCurrentIndex(0);
    this->ui->comboBoxExposureCompensator->setCurrentIndex(0);
    this->ui->spinBoxCompensatorBlockSize->setValue(32);
    this->ui->spinBoxCompensatorFeeds->setValue(1);
}


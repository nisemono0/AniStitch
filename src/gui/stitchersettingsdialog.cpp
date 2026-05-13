#include "gui/stitchersettingsdialog.hpp"

#include "utils/defs.hpp"


StitcherSettingsDialog::StitcherSettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::StitcherSettingsDialog) {
    this->ui->setupUi(this);

    // Wave correct combobox setup
    this->ui->comboBoxWaveCorrect->addItem(QStringLiteral("Horizontal"), WaveCorrectData::HORIZONTAL);
    this->ui->comboBoxWaveCorrect->addItem(QStringLiteral("Vertical"), WaveCorrectData::VERTICAL);
    this->ui->comboBoxWaveCorrect->addItem(QStringLiteral("Auto"), WaveCorrectData::AUTO);

    // Warper combobox setup
    this->ui->comboBoxWarper->addItem(QStringLiteral("Plane"), WarperData::PLANE);
    this->ui->comboBoxWarper->addItem(QStringLiteral("Spherical"), WarperData::SPHERICAL);
    this->ui->comboBoxWarper->addItem(QStringLiteral("Cylindrical"), WarperData::CYLINDRICAL);

    // Compensator combobox setup
    this->ui->comboBoxExposureCompensator->addItem(QStringLiteral("Blocks Gain"), CompensatorData::BLOCKS_GAIN);
    this->ui->comboBoxExposureCompensator->addItem(QStringLiteral("Blocks Channels"), CompensatorData::BLOCKS_CHANNEL);

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

    WaveCorrectData wave_correct = static_cast<WaveCorrectData>(this->ui->comboBoxWaveCorrect->currentData().toInt());
    switch (wave_correct) {
        case HORIZONTAL:
        {
            stitcher_settings.wave_correct = StitcherSettings::WaveCorrect::HORIZONTAL;
            break;
        }
        case VERTICAL:
        {
            stitcher_settings.wave_correct = StitcherSettings::WaveCorrect::VERTICAL;
            break;
        }
        case AUTO:
        {
            stitcher_settings.wave_correct = StitcherSettings::WaveCorrect::AUTO;
            break;
        }
    }

    WarperData warper = static_cast<WarperData>(this->ui->comboBoxWarper->currentData().toInt());
    switch (warper) {
        case PLANE:
        {
            stitcher_settings.warper = StitcherSettings::Warper::PLANE;
            break;
        }
        case SPHERICAL:
        {
            stitcher_settings.warper = StitcherSettings::Warper::SPHERICAL;
            break;
        }
        case CYLINDRICAL:
        {
            stitcher_settings.warper = StitcherSettings::Warper::CYLINDRICAL;
            break;
        }
    }

    CompensatorData compensator = static_cast<CompensatorData>(this->ui->comboBoxExposureCompensator->currentData().toInt());
    switch (compensator) {
        case BLOCKS_GAIN:
        {
            stitcher_settings.exposure_compensator = StitcherSettings::ExposureCompensator::BLOCKS_GAIN;
            break;
        }
        case BLOCKS_CHANNEL:
        {
            stitcher_settings.exposure_compensator = StitcherSettings::ExposureCompensator::BLOCKS_CHANNEL;
            break;
        }
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


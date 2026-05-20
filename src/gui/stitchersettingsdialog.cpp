#include "gui/stitchersettingsdialog.hpp"

#include "utils/defs.hpp"

#include <QKeyEvent>


StitcherSettingsDialog::StitcherSettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::StitcherSettingsDialog) {
    this->ui->setupUi(this);

    // Features finder combobox setup
    this->ui->comboBoxFeaturesFinder->addItem(QStringLiteral("SIFT"), FeaturesFinder::SIFT);
    this->ui->comboBoxFeaturesFinder->addItem(QStringLiteral("ORB"), FeaturesFinder::ORB);

    // Seam finder combobox setup
    this->ui->comboBoxSeamFinder->addItem(QStringLiteral("Cost color"), SeamFinder::COST_COLOR);
    this->ui->comboBoxSeamFinder->addItem(QStringLiteral("Cost color gradient"), SeamFinder::COST_COLOR_GRAD);

    // Wave correct combobox setup
    this->ui->comboBoxWaveCorrect->addItem(QStringLiteral("Horizontal"), WaveCorrectData::HORIZONTAL);
    this->ui->comboBoxWaveCorrect->addItem(QStringLiteral("Vertical"), WaveCorrectData::VERTICAL);
    this->ui->comboBoxWaveCorrect->addItem(QStringLiteral("Auto"), WaveCorrectData::AUTO);

    // Warper combobox setup
    this->ui->comboBoxWarper->addItem(QStringLiteral("Plane"), WarperData::PLANE);
    this->ui->comboBoxWarper->addItem(QStringLiteral("Spherical"), WarperData::SPHERICAL);
    this->ui->comboBoxWarper->addItem(QStringLiteral("Cylindrical"), WarperData::CYLINDRICAL);

    // Install event filter on widgets
    // Registration res
    this->ui->labelRegistrationRes->installEventFilter(this);
    this->ui->doubleSpinBoxRegistrationRes->installEventFilter(this);
    // Seam res
    this->ui->labelSeamRes->installEventFilter(this);
    this->ui->doubleSpinBoxSeamRes->installEventFilter(this);
    // Confidence threshold
    this->ui->labelConfidenceThreshold->installEventFilter(this);
    this->ui->doubleSpinBoxConfidenceThreshold->installEventFilter(this);
    // Features finder
    this->ui->labelFeaturesFinder->installEventFilter(this);
    this->ui->comboBoxFeaturesFinder->installEventFilter(this);
    // Features number
    this->ui->labelFeaturesNumber->installEventFilter(this);
    this->ui->spinBoxFeaturesNumber->installEventFilter(this);
    // Seam finder
    this->ui->labelSeamFinder->installEventFilter(this);
    this->ui->comboBoxSeamFinder->installEventFilter(this);
    // Blender bands
    this->ui->labelNumberBands->installEventFilter(this);
    this->ui->spinBoxNumberBands->installEventFilter(this);
    // Wave correct
    this->ui->labelWaveCorrect->installEventFilter(this);
    this->ui->comboBoxWaveCorrect->installEventFilter(this);
    // Warper
    this->ui->labelWarper->installEventFilter(this);
    this->ui->comboBoxWarper->installEventFilter(this);
    // Exposure compensator
    this->ui->labelExposureCompensator->installEventFilter(this);
    this->ui->comboBoxExposureCompensator->installEventFilter(this);
    // Compensator block size
    this->ui->labelCompensatorBlockSize->installEventFilter(this);
    this->ui->spinBoxCompensatorBlockSize->installEventFilter(this);
    // Compensator feeds
    this->ui->labelCompensatorFeeds->installEventFilter(this);
    this->ui->spinBoxCompensatorFeeds->installEventFilter(this);
    // Buttons
    this->ui->pushButtonClose->installEventFilter(this);
    this->ui->pushButtonResetValues->installEventFilter(this);
    this->ui->pushButtonStitchPanorama->installEventFilter(this);
    this->ui->pushButtonStitchScan->installEventFilter(this);

    // Default status tip message
    this->showDefaultStatusTip();

    // Compensator combobox setup
    this->ui->comboBoxExposureCompensator->addItem(QStringLiteral("Blocks Gain"), CompensatorData::BLOCKS_GAIN);
    this->ui->comboBoxExposureCompensator->addItem(QStringLiteral("Blocks Channels"), CompensatorData::BLOCKS_CHANNEL);

    connect(this->ui->pushButtonStitchScan, &QPushButton::clicked, this, &StitcherSettingsDialog::pushButtonStitchScan_clicked);
    connect(this->ui->pushButtonStitchPanorama, &QPushButton::clicked, this, &StitcherSettingsDialog::pushButtonStitchPanorama_clicked);
    connect(this->ui->pushButtonClose, &QPushButton::clicked, this, &QDialog::close);
    connect(this->ui->pushButtonResetValues, &QPushButton::clicked, this, &StitcherSettingsDialog::resetValues);
}

StitcherSettingsDialog::~StitcherSettingsDialog() {
    delete this->ui;
}

StitcherSettings StitcherSettingsDialog::getStitcherSettings() {
    StitcherSettings stitcher_settings;

    stitcher_settings.registration_res = this->ui->doubleSpinBoxRegistrationRes->value();
    stitcher_settings.seam_res = this->ui->doubleSpinBoxSeamRes->value();
    stitcher_settings.confidence_threshold = this->ui->doubleSpinBoxConfidenceThreshold->value();

    FeaturesFinder features_finder = static_cast<FeaturesFinder>(this->ui->comboBoxFeaturesFinder->currentData().toInt());
    switch (features_finder) {
        case SIFT:
        {
            stitcher_settings.features_finder = StitcherSettings::FeaturesFinder::SIFT;
            break;
        }
        case ORB:
        {
            stitcher_settings.features_finder = StitcherSettings::FeaturesFinder::ORB;
            break;
        }
    }

    stitcher_settings.features_finder_number = this->ui->spinBoxFeaturesNumber->value();

    SeamFinder seam_finder = static_cast<SeamFinder>(this->ui->comboBoxSeamFinder->currentData().toInt());
    switch (seam_finder) {
        case COST_COLOR:
        {
            stitcher_settings.seam_finder = StitcherSettings::SeamFinder::COST_COLOR;
            break;
        }
        case COST_COLOR_GRAD:
        {
            stitcher_settings.seam_finder = StitcherSettings::SeamFinder::COST_COLOR_GRAD;
            break;
        }
    }

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

void StitcherSettingsDialog::showDefaultStatusTip() {
    this->ui->labelStatusTip->setText(QStringLiteral("Custom settings for stitcher"));
}

bool StitcherSettingsDialog::eventFilter(QObject *o, QEvent *e) {
    auto widget = qobject_cast<QWidget *>(o);

    if (!widget) {
        return QDialog::eventFilter(o, e);
    }

    switch (e->type()) {
        // Mouse enters widget area
        case QEvent::Enter:
        {
            this->ui->labelStatusTip->setText(widget->statusTip());
            return true;
        }
        // Mouse leaves widget area
        case QEvent::Leave:
        {
            this->showDefaultStatusTip();
            return true;
        }
        // Keypress
        case QEvent::KeyPress:
        {
            QKeyEvent *key_event = static_cast<QKeyEvent*>(e);
            switch (key_event->key()) {
                case Qt::Key_Q:
                {
                    this->ui->pushButtonClose->animateClick();
                    return true;
                }
                case Qt::Key_R:
                {
                    this->ui->pushButtonResetValues->animateClick();
                    return true;
                }
                case Qt::Key_P:
                {
                    this->ui->pushButtonStitchPanorama->animateClick();
                    return true;
                }
                case Qt::Key_S:
                {
                    this->ui->pushButtonStitchScan->animateClick();
                    return true;
                }
                default:
                    break;
            }
        }
        default:
            break;
    }

    return QDialog::eventFilter(o, e);

}

void StitcherSettingsDialog::receive_show_StitcherSettingsDialog_request() {
    if (this->isVisible()) {
        this->raise();
        this->activateWindow();
    } else {
        this->resetValues();
        this->show();
    }
}

void StitcherSettingsDialog::pushButtonStitchScan_clicked(bool checked) {
    StitcherSettings stitcher_settings = this->getStitcherSettings();
    emit send_StitcherSettingsDialog_scan_settings(stitcher_settings);
}

void StitcherSettingsDialog::pushButtonStitchPanorama_clicked(bool checked) {
    StitcherSettings stitcher_settings = this->getStitcherSettings();
    emit send_StitcherSettingsDialog_panorama_settings(stitcher_settings);
}

void StitcherSettingsDialog::resetValues() {
    this->ui->doubleSpinBoxRegistrationRes->setValue(0.7);
    this->ui->doubleSpinBoxSeamRes->setValue(0.1);
    this->ui->doubleSpinBoxConfidenceThreshold->setValue(0.7);
    this->ui->comboBoxFeaturesFinder->setCurrentIndex(0);
    this->ui->spinBoxFeaturesNumber->setValue(2000);
    this->ui->comboBoxSeamFinder->setCurrentIndex(0);
    this->ui->spinBoxNumberBands->setValue(0);
    this->ui->comboBoxWaveCorrect->setCurrentIndex(0);
    this->ui->comboBoxWarper->setCurrentIndex(0);
    this->ui->comboBoxExposureCompensator->setCurrentIndex(0);
    this->ui->spinBoxCompensatorBlockSize->setValue(32);
    this->ui->spinBoxCompensatorFeeds->setValue(1);
}


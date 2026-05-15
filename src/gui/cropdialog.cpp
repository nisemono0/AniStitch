#include "gui/cropdialog.hpp"


CropDialog::CropDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CropDialog) {
    // Ui setup
    this->ui->setupUi(this);

    // Add event filter to widgets
    // Top input
    this->ui->labelCropTop->installEventFilter(this);
    this->ui->spinBoxCropTop->installEventFilter(this);
    // Bottom input
    this->ui->labelCropBottom->installEventFilter(this);
    this->ui->spinBoxCropBottom->installEventFilter(this);
    // Left input
    this->ui->labelCropLeft->installEventFilter(this);
    this->ui->spinBoxCropLeft->installEventFilter(this);
    // Right input
    this->ui->labelCropRight->installEventFilter(this);
    this->ui->spinBoxCropRight->installEventFilter(this);
    // Buttons
    this->ui->pushButtonReset->installEventFilter(this);
    this->ui->pushButtonClose->installEventFilter(this);
    this->ui->pushButtonCrop->installEventFilter(this);

    // Cancel/Reset buttons
    connect(this->ui->pushButtonClose, &QPushButton::clicked, this, &QDialog::close);
    connect(this->ui->pushButtonReset, &QPushButton::clicked, this, &CropDialog::resetValues);
    // Crop button pressed
    connect(this->ui->pushButtonCrop, &QPushButton::clicked, this, &CropDialog::pushButtonCrop_clicked);
}

CropDialog::~CropDialog() {
    delete this->ui;
}

bool CropDialog::eventFilter(QObject *o, QEvent *e) {
    auto widget = qobject_cast<QWidget *>(o);

    if (!widget) {
        return QDialog::eventFilter(o, e);
    }

    switch (e->type()) {
        case QEvent::Enter:
        {
            this->ui->labelStatusTip->setText(widget->statusTip());
            break;
        }
        case QEvent::Leave:
        {
            this->ui->labelStatusTip->clear();
            break;
        }
        default:
            break;
    }

    return QDialog::eventFilter(o, e);
}

void CropDialog::receive_show_CropDialog_request() {
    if (this->isVisible()) {
        this->raise();
        this->activateWindow();
    } else {
        this->resetValues();
        this->show();
    }
}

void CropDialog::resetValues() {
    this->ui->spinBoxCropTop->setValue(0);
    this->ui->spinBoxCropRight->setValue(0);
    this->ui->spinBoxCropBottom->setValue(0);
    this->ui->spinBoxCropLeft->setValue(0);
}

void CropDialog::pushButtonCrop_clicked(bool checked) {
    int top_px = this->ui->spinBoxCropTop->value();
    int right_px = this->ui->spinBoxCropRight->value();
    int bottom_px = this->ui->spinBoxCropBottom->value();
    int left_px = this->ui->spinBoxCropLeft->value();

    emit send_CropDialog_crop_values(top_px, right_px, bottom_px, left_px);
}


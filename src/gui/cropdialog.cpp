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
    this->ui->pushButtonCropValue->installEventFilter(this);
    this->ui->pushButtonCropSelection->installEventFilter(this);

    // Default status tip message
    this->ui->labelStatusTip->setText(QStringLiteral("You can make selections on the image"));

    // Cancel/Reset buttons
    connect(this->ui->pushButtonClose, &QPushButton::clicked, this, &CropDialog::close);
    connect(this->ui->pushButtonReset, &QPushButton::clicked, this, &CropDialog::pushButtonReset_clicked);
    // Crop button pressed
    connect(this->ui->pushButtonCropValue, &QPushButton::clicked, this, &CropDialog::pushButtonCropValue_clicked);
    connect(this->ui->pushButtonCropSelection, &QPushButton::clicked, this, &CropDialog::pushButtonCropSelection_clicked);
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
            this->ui->labelStatusTip->setText(QStringLiteral("You can make selections on the image"));
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

void CropDialog::pushButtonReset_clicked(bool checked) {
    this->resetValues();
    emit send_CropDialog_reset();
}

void CropDialog::pushButtonCropValue_clicked(bool checked) {
    int top_px = this->ui->spinBoxCropTop->value();
    int right_px = this->ui->spinBoxCropRight->value();
    int bottom_px = this->ui->spinBoxCropBottom->value();
    int left_px = this->ui->spinBoxCropLeft->value();

    emit send_CropDialog_crop_value(top_px, right_px, bottom_px, left_px);
}

void CropDialog::pushButtonCropSelection_clicked(bool checked) {
    emit send_CropDialog_crop_selection();
}


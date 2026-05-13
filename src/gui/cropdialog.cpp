#include "gui/cropdialog.hpp"


CropDialog::CropDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CropDialog) {
    this->ui->setupUi(this);

    // Cancel/Reset buttons
    connect(this->ui->pushButtonClose, &QPushButton::clicked, this, &QDialog::close);
    connect(this->ui->pushButtonReset, &QPushButton::clicked, this, &CropDialog::resetValues);
    // Crop button pressed
    connect(this->ui->pushButtonCrop, &QPushButton::clicked, this, &CropDialog::pushButtonCrop_clicked);
}

CropDialog::~CropDialog() {
    delete this->ui;
}

void CropDialog::receive_show_CropDialog_request() {
    this->show();
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


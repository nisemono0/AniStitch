#include "app/dualprogressdialog.hpp"


DualProgressDialog::DualProgressDialog(QWidget *parent) : QDialog(parent), ui(new Ui::DualProgressDialog) {
    // Ui setup
    this->ui->setupUi(this);

    // Make the progress dialog modal, this blocks
    // user input while the progressbar is shown
    this->setModal(true);

    // Make the window frameless
    this->setWindowFlag(Qt::FramelessWindowHint);
    // Make the dialog stay on top
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    // Delete this object on close
    this->setAttribute(Qt::WA_DeleteOnClose);
    // Set window cursor
    this->setCursor(Qt::BusyCursor);

    // Total progressbar
    this->ui->progressBarTotal->setRange(0, 100);
    this->ui->progressBarTotal->setValue(0);
    // Current progress bar
    this->ui->progressBarCurrent->setRange(0, 100);
    this->ui->progressBarCurrent->setValue(0);

    connect(this->ui->pushButtonCancel, &QPushButton::clicked, this, &DualProgressDialog::pushButtonCancel_clicked);
}

DualProgressDialog::~DualProgressDialog() {
    delete this->ui;
}

void DualProgressDialog::receive_reset_current_progress_request() {
    this->ui->progressBarCurrent->setValue(0);
}

void DualProgressDialog::receive_current_progress(int current_progress) {
    this->ui->progressBarCurrent->setValue(current_progress);
}

void DualProgressDialog::receive_total_progress(int total_progress) {
    this->ui->progressBarTotal->setValue(total_progress);
}

void DualProgressDialog::pushButtonCancel_clicked(bool checked) {
    this->close();
    emit cancel_progress();
}


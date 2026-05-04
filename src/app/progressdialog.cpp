#include "app/progressdialog.hpp"


ProgressDialog::ProgressDialog(QWidget *parent) : QProgressDialog(parent) {
    // Set window title and label text
    this->setWindowTitle(QStringLiteral("Progress"));
    this->setLabelText(QStringLiteral("Stitching images"));

    // Set fixed size for the dialog
    this->setFixedSize(this->size());

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

    // Set range and init value
    this->setRange(0, 100);
    this->setValue(0);
}

ProgressDialog::~ProgressDialog() {

}

void ProgressDialog::receive_progress(int progress) {
    this->setValue(progress);
}


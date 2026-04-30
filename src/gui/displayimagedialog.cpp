#include "gui/displayimagedialog.hpp"

#include "utils/opencv.hpp"
#include "utils/file.hpp"
#include "utils/str.hpp"
#include "utils/log.hpp"

#include <QMessageBox>


DisplayImageDialog::DisplayImageDialog(QWidget *parent) : QDialog(parent), ui(new Ui::DisplayImageDialog) {
    this->ui->setupUi(this);

    connect(this->ui->pushButtonSaveImage, &QPushButton::clicked, this, &DisplayImageDialog::pushButtonSaveImage_clicked);
    connect(this->ui->pushButtonClose, &QPushButton::clicked, this, &DisplayImageDialog::close);
}

DisplayImageDialog::~DisplayImageDialog() {
    delete this->ui;
}

void DisplayImageDialog::receive_show_DisplayImageDialog_request(const cv::Mat &cv_mat) {
    // Clear the scene and empty the stitched_cv_mat
    // before displaying a new stitched_cv_mat
    this->ui->graphicsDisplayImageView->clearScene();
    this->stitched_cv_mat = cv::Mat();

    QPixmap pixmap = Utils::Image::getPixmapFromMat(cv_mat);
    this->ui->graphicsDisplayImageView->displayPixmap(pixmap);

    // Keep the stitched cv::Mat to save it with opencv
    this->stitched_cv_mat = cv_mat;

    // Display this dialog with exec (blocking)
    this->exec();
}

void DisplayImageDialog::pushButtonSaveImage_clicked(bool checked) {
    QString save_image_path = Utils::FileDialog::saveImage(this);

    // Do nothing if save_image_path is null or empty
    if (Utils::String::isNullOrEmpty(save_image_path)) {
        return;
    }

    // Do nothing if stitched_cv_mat is empty
    if (this->stitched_cv_mat.empty()) {
        return;
    }

    bool image_save = Utils::Image::saveMatToPath(this->stitched_cv_mat, save_image_path);

    if (image_save) {
        Log::info(QStringLiteral("Image saved: %1").arg(save_image_path));
        QMessageBox::information(this, QStringLiteral("Save image"), QStringLiteral("Image saved"));
    } else {
        Log::error(QStringLiteral("Save image failed: %1").arg(save_image_path));
        QMessageBox::critical(this, QStringLiteral("Save image"), QStringLiteral("Save image failed; Check logs"));
    }
}


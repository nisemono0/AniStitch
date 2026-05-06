#include "gui/displayimagedialog.hpp"

#include "utils/opencv.hpp"
#include "utils/file.hpp"
#include "utils/str.hpp"
#include "utils/log.hpp"

#include <QMessageBox>


DisplayImageDialog::DisplayImageDialog(QWidget *parent) : QDialog(parent), ui(new Ui::DisplayImageDialog) {
    this->ui->setupUi(this);

    // Set selected tab to the be norma image tab
    this->ui->tabWidgetDisplayImages->setCurrentWidget(this->ui->tabNormalImage);

    // Save button
    connect(this->ui->pushButtonSaveImage, &QPushButton::clicked, this, &DisplayImageDialog::pushButtonSaveImage_clicked);
    // Close button
    connect(this->ui->pushButtonClose, &QPushButton::clicked, this, &DisplayImageDialog::close);
}

DisplayImageDialog::~DisplayImageDialog() {
    this->stitched_normal_mat.release();
    this->stitched_parsed_mat.release();
    this->stitched_thresholded_mat.release();
    delete this->ui;
}

void DisplayImageDialog::receive_show_DisplayImageDialog_request(const cv::Mat &normal_mat, const cv::Mat &parsed_mat, const cv::Mat &thresholded_mat) {
    // Clear the scenes and clone the mats
    this->ui->graphicsDisplayNormalImageView->clearScene();
    this->ui->graphicsDisplayParsedImageView->clearScene();
    this->ui->graphicsDisplayThresholdedImageView->clearScene();

    this->stitched_normal_mat = normal_mat.clone();
    this->stitched_parsed_mat = parsed_mat.clone();
    this->stitched_thresholded_mat = thresholded_mat.clone();

    // Convert the stitched mats to pixmaps to be displayed
    QPixmap normal_pixmap = Utils::Image::getPixmapFromMat(this->stitched_normal_mat);
    QPixmap parsed_pixmap = Utils::Image::getPixmapFromMat(this->stitched_parsed_mat);
    QPixmap thresholded_pixmap = Utils::Image::getPixmapFromMat(this->stitched_thresholded_mat);
    this->ui->graphicsDisplayNormalImageView->displayPixmap(normal_pixmap);
    this->ui->graphicsDisplayParsedImageView->displayPixmap(parsed_pixmap);
    this->ui->graphicsDisplayThresholdedImageView->displayPixmap(thresholded_pixmap);

    // Display this dialog with show (nonblocking)
    this->show();
}

void DisplayImageDialog::pushButtonSaveImage_clicked(bool checked) {
    QString save_image_path = Utils::FileDialog::saveImage(this);

    // Do nothing if save_image_path is null or empty
    if (Utils::String::isNullOrEmpty(save_image_path)) {
        return;
    }

    cv::Mat image_to_save;
    // Set image_to_save base on what tab is displaying the image
    if (this->ui->tabWidgetDisplayImages->currentWidget() == this->ui->tabNormalImage) {
        image_to_save = this->stitched_normal_mat;
    } else if (this->ui->tabWidgetDisplayImages->currentWidget() == this->ui->tabParsedImage) {
        image_to_save = this->stitched_parsed_mat;
    } else if (this->ui->tabWidgetDisplayImages->currentWidget() == this->ui->tabThresholdedImage) {
        image_to_save = this->stitched_thresholded_mat;
    }

    // Do nothing if image_to_save is empty
    if (image_to_save.empty()) {
        return;
    }

    bool is_saved = Utils::Image::saveMatToPath(image_to_save, save_image_path);
    if (is_saved) {
        Log::info(QStringLiteral("Image saved: %1").arg(save_image_path));
        QMessageBox::information(this, QStringLiteral("Save image"), QStringLiteral("Image saved"));
    } else {
        Log::error(QStringLiteral("Save image failed: %1").arg(save_image_path));
        QMessageBox::critical(this, QStringLiteral("Save image"), QStringLiteral("Save image failed; Check logs"));
    }
}


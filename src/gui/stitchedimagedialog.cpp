#include "gui/stitchedimagedialog.hpp"

#include "utils/opencv.hpp"
#include "utils/file.hpp"
#include "utils/str.hpp"
#include "utils/log.hpp"

#include <QMessageBox>
#include <QKeyEvent>


StitchedImageDialog::StitchedImageDialog(QWidget *parent) : QDialog(parent), ui(new Ui::StitchedImageDialog) {
    this->ui->setupUi(this);

    // Set selected tab to the be normal image tab
    this->ui->tabWidgetDisplayImages->setCurrentWidget(this->ui->tabNormalImage);

    // Add event filter to tab widget
    this->ui->tabWidgetDisplayImages->installEventFilter(this);

    // Add event filter to buttons
    this->ui->pushButtonSaveImage->installEventFilter(this);
    this->ui->pushButtonClose->installEventFilter(this);

    // Default labelStatusTip message
    this->showDefaultStatusTip();

    // Tab widget change
    connect(this->ui->tabWidgetDisplayImages, &QTabWidget::currentChanged, this, &StitchedImageDialog::tabWidgetDisplayImages_currentChanged);
    // Save button
    connect(this->ui->pushButtonSaveImage, &QPushButton::clicked, this, &StitchedImageDialog::pushButtonSaveImage_clicked);
    // Close button
    connect(this->ui->pushButtonClose, &QPushButton::clicked, this, &StitchedImageDialog::close);
}

StitchedImageDialog::~StitchedImageDialog() {
    this->stitched_normal_mat.release();
    this->stitched_parsed_mat.release();
    this->stitched_thresholded_mat.release();
    delete this->ui;
}

void StitchedImageDialog::showDefaultStatusTip() {
    this->ui->labelStatusTip->setText(QStringLiteral("Save stitched image"));
}

bool StitchedImageDialog::eventFilter(QObject *o, QEvent *e) {
    auto widget = qobject_cast<QWidget*>(o);

    if (!widget) {
        return QDialog::eventFilter(o, e);
    }

    switch (e->type()) {
        // Mouse enter widget area
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
            QKeyEvent *key_event = static_cast<QKeyEvent *>(e);
            switch (key_event->key()) {
                case Qt::Key_Tab:
                {
                    int next_tab = (this->ui->tabWidgetDisplayImages->currentIndex() + 1) % this->ui->tabWidgetDisplayImages->count();
                    this->ui->tabWidgetDisplayImages->setCurrentIndex(next_tab);
                    return true;
                }
                case Qt::Key_Backtab:
                {
                    int prev_tab = (this->ui->tabWidgetDisplayImages->currentIndex() - 1) % this->ui->tabWidgetDisplayImages->count();
                    this->ui->tabWidgetDisplayImages->setCurrentIndex(prev_tab);
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

void StitchedImageDialog::receive_show_DisplayImageDialog_request(const cv::Mat &normal_mat, const cv::Mat &parsed_mat, const cv::Mat &thresholded_mat) {
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

    // Set the current selected tab to the normal image one
    this->ui->tabWidgetDisplayImages->setCurrentWidget(this->ui->tabNormalImage);

    // Display this dialog with show or focus it if visible
    if (this->isVisible()) {
        this->raise();
        this->activateWindow();
    } else {
        this->show();
    }
}

void StitchedImageDialog::pushButtonSaveImage_clicked(bool checked) {
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
        Log::error(QStringLiteral("Failed to save image: %1").arg(save_image_path));
        QMessageBox::critical(this, QStringLiteral("Save image"), QStringLiteral("Could not save image"));
    }
}

void StitchedImageDialog::tabWidgetDisplayImages_currentChanged(int index) {
    // Change stauts tip based on what tab is pressed
    if (this->ui->tabWidgetDisplayImages->currentWidget() == this->ui->tabNormalImage) {
        this->ui->tabWidgetDisplayImages->setStatusTip(QStringLiteral("Default stitched image"));
        this->ui->labelStatusTip->setText(QStringLiteral("Default stitched image"));
    } else if (this->ui->tabWidgetDisplayImages->currentWidget() == this->ui->tabParsedImage) {
        this->ui->tabWidgetDisplayImages->setStatusTip(QStringLiteral("Stitched image with pure black pixels removed by parsing them from the outside in"));
        this->ui->labelStatusTip->setText(QStringLiteral("Stitched image with pure black pixels removed by parsing them from the outside in"));
    } else if (this->ui->tabWidgetDisplayImages->currentWidget() == this->ui->tabThresholdedImage) {
        this->ui->tabWidgetDisplayImages->setStatusTip(QStringLiteral("Stitched image with pure black pixels removed by using a binary threshold mask"));
        this->ui->labelStatusTip->setText(QStringLiteral("Stitched image with pure black pixels removed by using a binary threshold mask"));
    }
}


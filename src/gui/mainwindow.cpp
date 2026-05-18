#include "gui/mainwindow.hpp"

#include "utils/file.hpp"
#include "utils/str.hpp"
#include "utils/log.hpp"

#include <QMessageBox>
#include <QMimeData>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    // Ui setup
    this->ui->setupUi(this);
    this->log_window_dialog = new LogDialog(this);
    this->display_image_dialog = new StitchedImageDialog(this);
    this->stitcher_settings_dialog = new StitcherSettingsDialog(this);
    this->crop_dialog = new CropDialog(this);

    // Image list status label
    this->image_list_status = new QLabel(this);
    this->image_list_status->setText(QStringLiteral("No images loaded"));
    // Indent 4 makes it be aligned with the default statusbar message
    this->image_list_status->setIndent(4);
    this->ui->statusbar->addWidget(this->image_list_status);

    // ImageLoader worker/thread
    this->image_loader_worker = new ImageLoader();
    this->image_loader_thread = new QThread(this);
    this->image_loader_worker->moveToThread(this->image_loader_thread);

    // ImageStitcher worker/thread
    this->image_stitcher_worker = new ImageStitcher();
    this->image_stitcher_thread = new QThread(this);
    this->image_stitcher_worker->moveToThread(this->image_stitcher_thread);

    // Send signal to open the file(s) and start loading them
    connect(this->ui->pushButtonLoad, &QPushButton::clicked, this, &MainWindow::startImageLoad);
    // Send signal to ImageListView to remove the selected items from ImageListView
    connect(this->ui->pushButtonDeleteImage, &QPushButton::clicked, this->ui->imagesListView, &ImageListView::receive_delete_selected_items_request);
    // Send signal to ImageListView to remove all the items from ImageListView
    connect(this->ui->pushButtonClearImages, &QPushButton::clicked, this->ui->imagesListView, &ImageListView::receive_clear_items_request);
    // Send signal to ImageGraphicsView to clear the scene when the Clear button is pressed
    connect(this->ui->pushButtonClearImages, &QPushButton::clicked, this->ui->imageGraphicsView, &ImageGraphicsView::receive_clear_scene_request);
    // Send signal to ImageLoader to reset the image_counter to 0 when the Clear button is pressed
    connect(this->ui->pushButtonClearImages, &QPushButton::clicked, this->image_loader_worker, &ImageLoader::receive_ImageLoader_reset_counter);
    // Send signal to CropDialog to show the dialog
    connect(this->ui->pushButtonCropImages, &QPushButton::clicked, this->crop_dialog, &CropDialog::receive_show_CropDialog_request);
    // Enable crop selection when showing the CropDialog
    connect(this->ui->pushButtonCropImages, &QPushButton::clicked, this->ui->imageGraphicsView, &ImageGraphicsView::receive_enable_selection_request);
    // Disable crop selection on CropDialog reject
    // Rejection happen on ESC, Alt-F4, close() etc.
    connect(this->crop_dialog, &CropDialog::rejected, this->ui->imageGraphicsView, &ImageGraphicsView::receive_disable_selection_request);
    // Reset selection on CropDialog reset button press
    connect(this->crop_dialog, &CropDialog::send_CropDialog_reset, this->ui->imageGraphicsView, &ImageGraphicsView::receive_reset_selection_request);
    // Emit signal to forward the selection coords
    connect(this->crop_dialog, &CropDialog::send_CropDialog_crop_selection, this->ui->imageGraphicsView, &ImageGraphicsView::receive_send_selection_request);

    // CropDialog send crop values
    connect(this->crop_dialog, &CropDialog::send_CropDialog_crop_value, this->ui->imagesListView, &ImageListView::receive_crop_value_request);
    // CropdDialog send crop selection from ImageGraphicsView
    connect(this->ui->imageGraphicsView, &ImageGraphicsView::send_ImageGraphicsView_selection, this->ui->imagesListView, &ImageListView::receive_crop_selection_request);

    // ImageListView send status message to image_list_status
    connect(this->ui->imagesListView, &ImageListView::send_ImageListView_status, this->image_list_status, &QLabel::setText);

    // Stitch buttons
    connect(this->ui->pushButtonStitchScan, &QPushButton::clicked, this, &MainWindow::pushButtonStitchScan_clicked);
    connect(this->ui->pushButtonStitchPanorama, &QPushButton::clicked, this, &MainWindow::pushButtonStitchPanorama_clicked);
    connect(this->ui->pushButtonStitchCustom, &QPushButton::clicked, this->stitcher_settings_dialog, &StitcherSettingsDialog::receive_show_StitcherSettingsDialog_request);
    // Stitch settings buttons
    connect(this->stitcher_settings_dialog, &StitcherSettingsDialog::send_StitcherSettingsDialog_scan_settings, this, &MainWindow::receive_StitcherSettingsDialog_scan_settings);
    connect(this->stitcher_settings_dialog, &StitcherSettingsDialog::send_StitcherSettingsDialog_panorama_settings, this, &MainWindow::receive_StitcherSettingsDialog_panorama_settings);

    // Menubar:File
    connect(this->ui->actionLoad, &QAction::triggered, this, &MainWindow::startImageLoad);
    connect(this->ui->actionQuit, &QAction::triggered, this, &QApplication::quit);

    // Menubar:Info
    connect(this->ui->actionShowLogs, &QAction::triggered, this->log_window_dialog, &LogDialog::receive_show_LogDialog_request);
    connect(this->ui->actionShowAbout, &QAction::triggered, this, &QApplication::aboutQt);

    // ImageLoader worker thread
    connect(this, &MainWindow::request_ImageLoader_start, this->image_loader_worker, &ImageLoader::receive_ImageLoader_start_request);
    connect(this->image_loader_worker, &ImageLoader::send_ImageLoader_status, this->image_loader_thread, &QThread::quit);
    // Receive ImageLoader status
    connect(this->image_loader_worker, &ImageLoader::send_ImageLoader_status, this, &MainWindow::receive_ImageLoader_status);
    // Receive ImageLoader show progress bar request
    connect(this->image_loader_worker, &ImageLoader::send_ImageLoader_show_progress_bar, this, &MainWindow::receive_ImageLoader_show_progress_bar);
    // Send ImageLoader data to ImageListView
    connect(this->image_loader_worker, &ImageLoader::send_ImageLoader_data, this->ui->imagesListView, &ImageListView::receive_insert_item_request);
    // Send the current selected ImageListView item's pixmap to ImageGraphicsView to be displayed
    connect(this->ui->imagesListView, &ImageListView::send_current_item_pixmap, this->ui->imageGraphicsView, &ImageGraphicsView::receive_show_pixmap_request);

    // ImageStitcher worker thread
    connect(this, &MainWindow::request_ImageStitcher_start, this->image_stitcher_worker, &ImageStitcher::receive_ImageStitcher_start_request);
    connect(this->image_stitcher_worker, &ImageStitcher::send_ImageStitcher_status, this->image_stitcher_thread, &QThread::quit);
    // Receive ImageStitcher status
    connect(this->image_stitcher_worker, &ImageStitcher::send_ImageStitcher_status, this, &MainWindow::receive_ImageStitcher_status);
    // Receive ImageStitcher show progress bar request
    connect(this->image_stitcher_worker, &ImageStitcher::send_ImageStitcher_show_progress_bar, this, &MainWindow::receive_ImageStitcher_show_progress_bar);
    // Send ImageStitcher stitched image to the StitchedImageDialog to be displayed
    connect(this->image_stitcher_worker, &ImageStitcher::send_ImageStitcher_data, this->display_image_dialog, &StitchedImageDialog::receive_show_DisplayImageDialog_request);

    // Receive ImageGraphicsScene drag and drop events
    connect(this->ui->imageGraphicsView->getGraphicsScene(), &ImageGraphicsScene::send_ImageGraphicsScene_drag_drop_file_paths, this, &MainWindow::receive_ImageGraphicsScene_drag_drop_file_paths);
}

MainWindow::~MainWindow() {
    if (this->image_loader_thread->isRunning()) {
        this->image_loader_thread->requestInterruption();
        this->image_loader_thread->quit();
        this->image_loader_thread->wait(1000);
        if (this->image_loader_thread->isRunning()) {
            this->image_loader_thread->terminate();
            this->image_loader_thread->wait();
        }
        this->image_loader_thread->deleteLater();
    }

    if (this->image_stitcher_thread->isRunning()) {
        this->image_stitcher_thread->requestInterruption();
        this->image_stitcher_thread->quit();
        this->image_stitcher_thread->wait(1000);
        if (this->image_stitcher_thread->isRunning()) {
            this->image_stitcher_thread->terminate();
            this->image_stitcher_thread->wait();
        }
        this->image_stitcher_thread->deleteLater();
    }

    delete this->image_loader_worker;
    delete this->image_loader_thread;

    delete this->image_stitcher_worker;
    delete this->image_stitcher_thread;

    delete this->image_loader_progress_dialog;
    delete this->image_stitcher_progress_dialog;

    delete this->image_list_status;

    delete this->crop_dialog;
    delete this->stitcher_settings_dialog;
    delete this->log_window_dialog;
    delete this->display_image_dialog;

    delete this->ui;
}

void MainWindow::startImageLoad() {
    // If the ImageLoader thread is running, show a
    // message and bring the progress bar on front
    if (this->image_loader_thread->isRunning()) {
        // Bring the progress dialog on front again
        if (this->image_loader_progress_dialog) {
            this->image_loader_progress_dialog->raise();
            this->image_loader_progress_dialog->activateWindow();
        }
        QMessageBox::information(this, QStringLiteral("Load file(s)"), QStringLiteral("Already running"));
        return;
    }

    QStringList selected_files = Utils::FileDialog::openFile(this);
    if (selected_files.isEmpty()) {
        return;
    }

    // Start the thread and send work
    this->image_loader_thread->start();
    emit request_ImageLoader_start(selected_files);
}

void MainWindow::startImageStitch(ImageStitcher::ImageStitcherType stitcher_type, const StitcherSettings &stitcher_settings) {
    // If the ImageStitcher thread is running, show a
    // message and bring the progress bar on front
    if (this->image_stitcher_thread->isRunning()) {
        // Bring the progress dialog on front again
        if (this->image_stitcher_progress_dialog) {
            this->image_stitcher_progress_dialog->raise();
            this->image_stitcher_progress_dialog->activateWindow();
        }
        QMessageBox::information(this, QStringLiteral("Image stitcher"), QStringLiteral("Already running"));
        return;
    }

    // Get all the cv_mats from the ImageListView
    std::vector<cv::Mat> cv_mats = this->ui->imagesListView->getItemsCVMats();

    if (cv_mats.size() < 2) {
        QMessageBox::information(this, QStringLiteral("Image stitcher"), QStringLiteral("Need at least 2 images"));
        return;
    }

    // Start the thread and send work
    this->image_stitcher_thread->start();
    emit request_ImageStitcher_start(cv_mats, stitcher_type, stitcher_settings);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event) {
    const QMimeData *mime_data = event->mimeData();

    if (mime_data->hasUrls()) {
        QStringList file_paths = QStringList();

        // Get local file paths from mime_data urls
        for (auto &url : mime_data->urls()) {
            QString local_url = url.toLocalFile();
            if (!Utils::String::isNullOrEmpty(local_url)) {
                file_paths.append(local_url);
            }
        }
        // Start image loader thread on file_paths
        if (!this->image_loader_thread->isRunning()) {
            this->image_loader_thread->start();
            emit request_ImageLoader_start(file_paths);
        }
    }

    event->acceptProposedAction();
}

void MainWindow::receive_ImageGraphicsScene_drag_drop_file_paths(const QStringList &file_paths) {
    // Start image loader thread on file_paths
    if (!this->image_loader_thread->isRunning()) {
        this->image_loader_thread->start();
        emit request_ImageLoader_start(file_paths);
    }
}

void MainWindow::receive_StitcherSettingsDialog_scan_settings(const StitcherSettings &stitcher_settings) {
    this->startImageStitch(ImageStitcher::ImageStitcherType::CUSTOM_SCAN, stitcher_settings);
}

void MainWindow::receive_StitcherSettingsDialog_panorama_settings(const StitcherSettings &stitcher_settings) {
    this->startImageStitch(ImageStitcher::ImageStitcherType::CUSTOM_PANORAMA, stitcher_settings);
}

void MainWindow::receive_ImageLoader_status(ImageLoader::ImageLoaderStatus status) {
    // Close the progress dialog if it exists
    if (this->image_loader_progress_dialog) {
        this->image_loader_progress_dialog->close();
    }

    switch (status) {
        case ImageLoader::ImageLoaderStatus::OK:
        {
            QMessageBox::information(this, QStringLiteral("Load file(s)"), QStringLiteral("All files loaded"));
            break;
        }
        case ImageLoader::ImageLoaderStatus::WARNING:
        {
            QMessageBox::warning(this, QStringLiteral("Load file(s)"), QStringLiteral("Warnings found; Check logs"));
            break;
        }
        case ImageLoader::ImageLoaderStatus::ERROR:
        {
            QMessageBox::critical(this, QStringLiteral("Load file(s)"), QStringLiteral("Errors found; Check logs"));
            break;
        }
        case ImageLoader::ImageLoaderStatus::INTERRUPTED:
        {
            QMessageBox::warning(this, QStringLiteral("Load file(s)"), QStringLiteral("File load canceled"));
            break;
        }
    }

}

void MainWindow::receive_ImageLoader_show_progress_bar() {
    if (this->image_loader_progress_dialog) {
        this->image_loader_progress_dialog->show();
        this->image_loader_progress_dialog->raise();
        this->image_loader_progress_dialog->activateWindow();
    } else {
        this->image_loader_progress_dialog = new DualProgressDialog(this);
        this->image_loader_progress_dialog->show();
        this->image_loader_progress_dialog->raise();
        this->image_loader_progress_dialog->activateWindow();
        // Those should disconnect automatically when image_loader_progress_dialog
        // or image_loader_thread or image_loader_worker gets deleted or when
        // image_loader_progress_dialog is closed
        connect(this->image_loader_progress_dialog, &DualProgressDialog::cancel_progress,
                this->image_loader_thread, [this]() {
                    if (this->image_loader_thread->isRunning()) {
                        this->image_loader_thread->requestInterruption();
                    }
                }
               );
        connect(this->image_loader_worker, &ImageLoader::send_ImageLoader_reset_current_progress,
                this->image_loader_progress_dialog, &DualProgressDialog::receive_reset_current_progress_request);
        connect(this->image_loader_worker, &ImageLoader::send_ImageLoader_current_progress,
                this->image_loader_progress_dialog, &DualProgressDialog::receive_current_progress);
        connect(this->image_loader_worker, &ImageLoader::send_ImageLoader_total_progress,
                this->image_loader_progress_dialog, &DualProgressDialog::receive_total_progress);
    }
}

void MainWindow::receive_ImageStitcher_status(ImageStitcher::ImageStitcherStatus status) {
    // Close the progress dialog if it exists
    if (this->image_stitcher_progress_dialog) {
        this->image_stitcher_progress_dialog->close();
    }

    switch (status) {
        case ImageStitcher::ImageStitcherStatus::OK:
        {
            QMessageBox::information(this, QStringLiteral("Image stitcher"), QStringLiteral("Done stitching"));
            break;
        }
        case ImageStitcher::ImageStitcherStatus::EXCEPTION:
        {
            QMessageBox::critical(this, QStringLiteral("Image Stitcher"), QStringLiteral("Error: worker exception"));
            break;
        }
        case ImageStitcher::ImageStitcherStatus::NOT_DONE:
        {
            QMessageBox::warning(this, QStringLiteral("Image stitcher"), QStringLiteral("Not all images stitched"));
            break;
        }
        case ImageStitcher::ImageStitcherStatus::NEED_MORE_IMGS:
        {
            QMessageBox::critical(this, QStringLiteral("Image stitcher"), QStringLiteral("Error: need more images"));
            break;
        }
        case ImageStitcher::ImageStitcherStatus::EST_FAIL:
        {
            QMessageBox::critical(this, QStringLiteral("Image stitcher"), QStringLiteral("Error: camera estimation failed"));
            break;
        }
        case ImageStitcher::ImageStitcherStatus::ADJUST_FAIL:
        {
            QMessageBox::critical(this, QStringLiteral("Image stitcher"), QStringLiteral("Error: camera adjusting failed"));
            break;
        }
        case ImageStitcher::ImageStitcherStatus::INTERRUPTED:
        {
            QMessageBox::information(this, QStringLiteral("Image stitcher"), QStringLiteral("Stitching canceled"));
            break;
        }
    }

}

void MainWindow::receive_ImageStitcher_show_progress_bar() {
    if (this->image_stitcher_progress_dialog) {
        this->image_stitcher_progress_dialog->show();
        this->image_stitcher_progress_dialog->raise();
        this->image_stitcher_progress_dialog->activateWindow();
    } else {
        this->image_stitcher_progress_dialog = new ProgressDialog(this, true);
        this->image_stitcher_progress_dialog->show();
        this->image_stitcher_progress_dialog->raise();
        this->image_stitcher_progress_dialog->activateWindow();
        // Those should disconnect automatically when image_stitcher_progress_dialog
        // or image_stitcher_thread or image_stitcher_worker gets deleted or when
        // image_stitcher_progress_dialog is closed
        connect(this->image_stitcher_progress_dialog, &ProgressDialog::canceled,
                this->image_stitcher_thread, [this]() {
                    if (this->image_stitcher_thread->isRunning()) {
                        this->image_stitcher_thread->requestInterruption();
                    }
                });
        connect(this->image_stitcher_worker, &ImageStitcher::send_ImageStitcher_progress,
                this->image_stitcher_progress_dialog, &ProgressDialog::receive_progress);
    }
}

void MainWindow::pushButtonStitchScan_clicked(bool checked) {
    this->startImageStitch(ImageStitcher::ImageStitcherType::SCAN, StitcherSettings());
}

void MainWindow::pushButtonStitchPanorama_clicked(bool checked) {
    this->startImageStitch(ImageStitcher::ImageStitcherType::PANORAMA, StitcherSettings());
}


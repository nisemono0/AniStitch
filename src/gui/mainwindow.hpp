#pragma once

#include "ui/ui_mainwindow.h"

#include "gui/logdialog.hpp"
#include "gui/stitchedimagedialog.hpp"
#include "gui/stitchersettingsdialog.hpp"
#include "gui/cropdialog.hpp"

#include "app/dualprogressdialog.hpp"
#include "app/progressdialog.hpp"

#include "base/imageloader.hpp"
#include "base/imagestitcher.hpp"

#include <QWidget>
#include <QMainWindow>
#include <QPointer>
#include <QThread>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QProgressDialog>


namespace Ui {
    class MainWindow;
};

class MainWindow : public QMainWindow {
    Q_OBJECT;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    // Ui
    Ui::MainWindow *ui;
    // Progress dialog for ImageLoader
    QPointer<DualProgressDialog> image_loader_progress_dialog;
    // Progress dialog for ImageStitcher
    QPointer<ProgressDialog> image_stitcher_progress_dialog;
    // Log window dialog
    LogDialog *log_window_dialog;
    // Display stitched image dialog
    StitchedImageDialog *display_image_dialog;
    // Stitcher settings dialog
    StitcherSettingsDialog *stitcher_settings_dialog;
    // Crop dialog
    CropDialog *crop_dialog;

    // ImageList status label
    QLabel *image_list_status;

    // Loader
    ImageLoader *image_loader_worker;
    QThread *image_loader_thread;

    // Stitcher
    ImageStitcher *image_stitcher_worker;
    QThread *image_stitcher_thread;

    // Get ImageList images and starts ImageStitcher thread.
    // Images are stitched using stitcher_type
    void startImageStitch(ImageStitcher::ImageStitcherType stitcher_type, const StitcherSettings &stitcher_settings);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void request_ImageLoader_start(const QStringList &file_paths);
    void request_ImageStitcher_start(const std::vector<cv::Mat> &cv_mats, const ImageStitcher::ImageStitcherType stitcher_type, const StitcherSettings &stitcher_settings);

public slots:
    // Image loader
    void receive_ImageLoader_status(ImageLoader::ImageLoaderStatus status);
    void receive_ImageLoader_show_progress_bar();

    // Image stitcher
    void receive_ImageStitcher_status(ImageStitcher::ImageStitcherStatus status);
    void receive_ImageStitcher_show_progress_bar();

    // Receive forwarded ImageGraphicsScene drag and dropped QList<QUrl> url_list
    void receive_ImageGraphicsScene_drag_drop_file_paths(const QStringList &file_paths);

    // Receive settings from the StitcherSettingsDialog
    void receive_StitcherSettingsDialog_scan_settings(const StitcherSettings &stitcher_settings);
    void receive_StitcherSettingsDialog_panorama_settings(const StitcherSettings &stitcher_settings);

private slots:
    // Load file(s)
    // Opens a file dialog and starts ImageLoader thread with them
    void startImageLoad();

    // Stitch buttons clicked
    void pushButtonStitchScan_clicked(bool checked = false);
    void pushButtonStitchPanorama_clicked(bool checked = false);
};


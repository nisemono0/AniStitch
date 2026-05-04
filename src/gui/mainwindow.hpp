#pragma once

#include "ui/ui_mainwindow.h"

#include "gui/logdialog.hpp"
#include "gui/displayimagedialog.hpp"

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
    DisplayImageDialog *display_image_dialog;

    // Loader
    ImageLoader *image_loader_worker;
    QThread *image_loader_thread;

    // Stitcher
    ImageStitcher *image_stitcher_worker;
    QThread *image_stitcher_thread;

    // Get ImageList images and starts ImageStitcher thread
    void startImageStitch();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

signals:
    void request_ImageLoader_start(const QStringList &file_paths);
    void request_ImageStitcher_start(const std::vector<cv::Mat> &cv_mats);

public slots:
    // Image loader
    void receive_ImageLoader_status(ImageLoader::ImageLoaderStatus status);
    void receive_ImageLoader_show_progress_bar();

    // Image stitcher
    void receive_ImageStitcher_status(ImageStitcher::ImageStitcherStatus status);
    void receive_ImageStitcher_show_progress_bar();

    // Receive forwarded ImageGraphicsScene drag and dropped QList<QUrl> url_list
    void receive_ImageGraphicsScene_drag_drop_file_paths(const QStringList &file_paths);

private slots:
    // Load file(s)
    // Opens a file dialog and starts ImageLoader thread with them
    void startImageLoad();

    // Stitch buttons clicked
    void pushButtonStitchImages_clicked(bool checked = false);
};


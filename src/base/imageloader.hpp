#pragma once

#include "base/imageitem.hpp"

#include <optional>

#include <QObject>


class ImageLoader : public QObject {
    Q_OBJECT;
public:
    // Statuses to send back
    enum ImageLoaderStatus { OK, EXCEPTION, WARNING, ERROR, INTERRUPTED };

    explicit ImageLoader(QObject *parent = nullptr);
    ~ImageLoader();

private:
    // Counter for number of images loaded
    // Used to set name of item as Image %loaded_counter%
    int image_counter;

    // Start worker
    void startWorker(const QStringList &file_paths);

    // Return a QString with a name based on current loader_count and update it
    QString getNameAndUpdateCounter();
    // Return an ImageItem from an image at image_path or std::nullopt
    std::optional<ImageItem> getImageItemFromImage(const QString &image_path);
    // Return a QList of std::optional<ImageItem> from a video at video_path
    // Each ImageItem inside the QList is a frame from the video, can be std::nullopt
    // if it could not be loaded
    QList<std::optional<ImageItem>> getImageItemFromVideo(const QString &video_path);

signals:
    // Send the finish status of the worker; this implies
    // that the thread the worker is in will also quit
    void send_ImageLoader_status(ImageLoader::ImageLoaderStatus status);
    // Sends a signal to start showing the progress bar
    void send_ImageLoader_show_progress_bar();
    // Send restart signal to reset the per file progress bar
    void send_ImageLoader_reset_current_progress();
    // Send current single file progress
    void send_ImageLoader_current_progress(int current_progress);
    // Send total processed files progress
    void send_ImageLoader_total_progress(int total_progress);
    // Send loaded ImageItem data
    void send_ImageLoader_data(const ImageItem &image);

public slots:
    // Receive request to start the image loader on file_paths
    void receive_ImageLoader_start_request(const QStringList &file_paths);
    // Receive request to reset loaded_counter;
    // This should be called when the Clear button is pressed
    void receive_ImageLoader_reset_counter();
};


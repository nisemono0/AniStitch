#pragma once

#include "utils/metatypes.hpp"

#include <opencv2/core.hpp>
#include <opencv2/stitching.hpp>

#include <expected>

#include <QObject>
#include <QPixmap>
#include <QThreadPool>


class ImageStitcher : public QObject {
    Q_OBJECT;
public:
    // Status to send back
    enum ImageStitcherStatus { OK, EXCEPTION, NOT_DONE, NEED_MORE_IMGS, EST_FAIL, ADJUST_FAIL, INTERRUPTED };
    // Stitch type
    enum ImageStitcherType { SCAN, PANORAMA, CUSTOM_SCAN, CUSTOM_PANORAMA };

    explicit ImageStitcher(QObject *parent = nullptr);
    ~ImageStitcher();

private:
    // Chunk sizes
    static const int DEFAULT_CHUNK_SIZE = 10;
    static const int SCAN_CHUNKS_SIZE = 10;
    static const int PANORAMA_CHUNKS_SIZE = 5;

    // Stitching thread pool
    QThreadPool *thread_pool;

    // Start worker
    void startWorker(const std::vector<cv::Mat> &cv_mats, ImageStitcher::ImageStitcherType stitcher_type, const StitcherSettings &stitcher_settings);

    // Returns a cv::Ptr of type cv::Stitcher with default SCAN settings
    cv::Ptr<cv::Stitcher> getScanStitcherPtr();
    // Returns a cv::Ptr of type cv::Stitcher with default PANORAMA settings
    cv::Ptr<cv::Stitcher> getPanoramaStitcherPtr();
    // Returns a cv::Ptr of type cv::Stitcher with custom settings in SCAN mode
    cv::Ptr<cv::Stitcher> getCustomScanStitcherPtr(const StitcherSettings &stitcher_settings);
    // Returns a cv::Ptr of type cv::Stitcher with custom settings in PANORAMA mode
    cv::Ptr<cv::Stitcher> getCustomPanoramaStitcherPtr(const StitcherSettings &stitcher_settings);
    // Returns a cv::Mat representing the stitched image or cv::Stitcher::Status on error
    // Uses stitche_type and stitcher_settings when needed
    std::expected<cv::Mat, cv::Stitcher::Status> stitchImages(const std::vector<cv::Mat> &cv_mats, ImageStitcher::ImageStitcherType stitcher_type, const StitcherSettings &stitcher_settings);
    // Split cv::Mat vector into chunks of chunk_size
    std::vector<std::vector<cv::Mat>> splitIntoChunks(const std::vector<cv::Mat> &cv_mats, int chunk_size);
    // Return total steps the program needs to do
    // Used to compute emit progress
    int getTotalProgressToDo(int size, int chunk_size);

signals:
    // Send back status of the worker based on the result of cv::Stitcher
    void send_ImageStitcher_status(ImageStitcher::ImageStitcherStatus status);
    // Send a signal to start showing the progress bar
    void send_ImageStitcher_show_progress_bar();
    // Send ImageStitcher progress
    void send_ImageStitcher_progress(int progress);
    // Send resulted stitched images
    void send_ImageStitcher_data(const cv::Mat &normal_mat, const cv::Mat &parsed_mat, const cv::Mat &thresholded_mat);

public slots:
    // Receive request to start stitching the cv_mats images given stitcher_type
    // and stitcher_settings
    void receive_ImageStitcher_start_request(const std::vector<cv::Mat> &cv_mats, ImageStitcher::ImageStitcherType stitcher_type, const StitcherSettings &stitcher_settings);
};


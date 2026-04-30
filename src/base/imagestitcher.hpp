#pragma once

#include <opencv2/core.hpp>
#include <opencv2/stitching.hpp>

#include <QObject>
#include <QPixmap>


class ImageStitcher : public QObject {
    Q_OBJECT;
public:
    // Type of stitch to do
    enum ImageStitcherType { Panorama, Scan };
    // Status to send back
    enum ImageStitcherStatus { Ok, NEED_MORE_IMGS, EST_FAIL, PARAMS_ADJUST_FAIL, Interrupted };

    explicit ImageStitcher(QObject *parent = nullptr);
    ~ImageStitcher();

private:
    // Start Panorama stitch on cv_mats
    void startPanoramaStitch(const std::vector<cv::Mat> &cv_mats);
    // Start Scan stitch on cv_mats
    void startScanStitch(const std::vector<cv::Mat> &cv_mats);

signals:
    // Send back status of the worker based on the result of cv::Stitcher
    void send_ImageStitcher_status(ImageStitcher::ImageStitcherStatus status);;
    // Send a signal to start showing the progress bar
    void send_ImageStitcher_show_progress_bar();
    // Send ImageStitcher progress
    void send_ImageStitcher_progress(int progress);
    // Send resulted stitched cv::Mat
    void send_ImageStitcher_data(const cv::Mat &stitched_cvmat);

public slots:
    // Receive request to start stitching the cv_mats images using stitcher_type
    void receive_ImageStitcher_start_request(const std::vector<cv::Mat> &cv_mats, ImageStitcher::ImageStitcherType stitcher_type);
};


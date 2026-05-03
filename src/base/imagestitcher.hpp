#pragma once

#include <opencv2/core.hpp>
#include <opencv2/stitching.hpp>

#include <QObject>
#include <QPixmap>
#include <expected>


class ImageStitcher : public QObject {
    Q_OBJECT;
public:
    // Status to send back
    enum ImageStitcherStatus { OK, NEED_MORE_IMGS, EST_FAIL, PARAMS_ADJUST_FAIL, INTERRUPTED };

    explicit ImageStitcher(QObject *parent = nullptr);
    ~ImageStitcher();

private:
    // Returns a cv::Ptr of type cv::Stitcher with custom settings
    cv::Ptr<cv::Stitcher> getStitcherPtr();
    // Start stitching the cv_mats
    // Returns a stitched cv::Mat or cv::Stitcher::Status on errors
    std::expected<cv::Mat, cv::Stitcher::Status> stitchImages(const std::vector<cv::Mat> &cv_mats);

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
    // Receive request to start stitching the cv_mats images
    void receive_ImageStitcher_start_request(const std::vector<cv::Mat> &cv_mats);
};


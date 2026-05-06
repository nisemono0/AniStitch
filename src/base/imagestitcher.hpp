#pragma once

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
    enum ImageStitcherStatus { OK, NOT_DONE, NEED_MORE_IMGS, EST_FAIL, ADJUST_FAIL, INTERRUPTED };

    explicit ImageStitcher(QObject *parent = nullptr);
    ~ImageStitcher();

private:
    cv::UMat final_mask;
    // How many images to stitch at once
    const int STITCHER_CHUNK_SIZE = 10;
    // Stitching thread pool
    QThreadPool *thread_pool;

    // Returns a cv::Ptr of type cv::Stitcher with custom settings
    cv::Ptr<cv::Stitcher> getStitcherPtr();
    // Returns a cv::Mat representing the stitched image or cv::Stitcher::Status on error
    std::expected<cv::Mat, cv::Stitcher::Status> stitchImages(const std::vector<cv::Mat> &cv_mats);
    // Split cv::Mat vector into STITCHER_CHUNK_SIZE chunks
    std::vector<std::vector<cv::Mat>> splitIntoChunks(const std::vector<cv::Mat> &cv_mats);
    // Return total steps the program needs to do
    // Used to compute emit progress
    int getTotalProgressToDo(int size, int chunk_size);

signals:
    // Send back status of the worker based on the result of cv::Stitcher
    void send_ImageStitcher_status(ImageStitcher::ImageStitcherStatus status);;
    // Send a signal to start showing the progress bar
    void send_ImageStitcher_show_progress_bar();
    // Send ImageStitcher progress
    void send_ImageStitcher_progress(int progress);
    // Send resulted stitched images
    void send_ImageStitcher_data(const cv::Mat &normal_mat, const cv::Mat &parsed_mat, const cv::Mat &thresholded_mat);

public slots:
    // Receive request to start stitching the cv_mats images
    void receive_ImageStitcher_start_request(const std::vector<cv::Mat> &cv_mats);
};


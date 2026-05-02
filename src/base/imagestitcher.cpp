#include "base/imagestitcher.hpp"

#include "utils/log.hpp"


ImageStitcher::ImageStitcher(QObject *parent) : QObject(parent) {
    // TODO: Set some stitcher settings or split the pipeline
    this->stitcher = cv::Stitcher::create(cv::Stitcher::SCANS);
}

ImageStitcher::~ImageStitcher() {

}

void ImageStitcher::stitchImages(const std::vector<cv::Mat> &cv_mats) {

}

void ImageStitcher::receive_ImageStitcher_start_request(const std::vector<cv::Mat> &cv_mats) {
    // TODO: Stitch here
    //       - emit send_ImageStitcher_show_progress_bar
    //       - Start stitching:
    //          - split in batches
    //          - call startImageStitch on batches
    //          - emit send_ImageStitcher_progress to display the progress
    //       - When done:
    //          - emit send_ImageStitcher_data
    //          - emit send_ImageStitcher_status
}


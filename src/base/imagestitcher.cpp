#include "base/imagestitcher.hpp"

#include "utils/log.hpp"


ImageStitcher::ImageStitcher(QObject *parent) : QObject(parent) {

}

ImageStitcher::~ImageStitcher() {

}

void ImageStitcher::startPanoramaStitch(const std::vector<cv::Mat> &cv_mats) {
    // TODO: Stitch in Panorama mode
    //       - emit send_ImageStitcher_show_progress_bar
    //       - Start stitching:
    //          - emit send_ImageStitcher_progress to display the progress
    //       - When done:
    //          - emit send_ImageStitcher_data
    //          - emit send_ImageStitcher_status
}

void ImageStitcher::startScanStitch(const std::vector<cv::Mat> &cv_mats) {
    // TODO: Stitch in Scan mode
    //       - emit send_ImageStitcher_show_progress_bar
    //       - Start stitching:
    //          - emit send_ImageStitcher_progress to display the progress
    //       - When done:
    //          - emit send_ImageStitcher_data
    //          - emit send_ImageStitcher_status
}


void ImageStitcher::receive_ImageStitcher_start_request(const std::vector<cv::Mat> &cv_mats, ImageStitcher::ImageStitcherType stitcher_type) {
    switch (stitcher_type) {
        case ImageStitcherType::Panorama:
        {
            this->startPanoramaStitch(cv_mats);
            break;
        }
        case ImageStitcherType::Scan:
        {
            this->startScanStitch(cv_mats);
            break;
        }
        default:
            return;
    }
}


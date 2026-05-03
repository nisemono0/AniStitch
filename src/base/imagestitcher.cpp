#include "base/imagestitcher.hpp"

#include "utils/log.hpp"
#include "utils/opencv.hpp"

#include <QtConcurrent>


ImageStitcher::ImageStitcher(QObject *parent) : QObject(parent) {

}

ImageStitcher::~ImageStitcher() {

}

cv::Ptr<cv::Stitcher> ImageStitcher::getStitcherPtr() {
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::SCANS);

    // Set image registration resolution
    stitcher->setRegistrationResol(0.6);
    // Set image resolution for seam estimation
    stitcher->setSeamEstimationResol(0.1);
    // Set resolution for final stitch
    stitcher->setCompositingResol(cv::Stitcher::ORIG_RESOL);
    // Confidence threshold for images to be part of the same stitch (Default: 1)
    stitcher->setPanoConfidenceThresh(0.7);
    // Seam finder:
    //      - GraphCutSeamFinder:
    //          - COST_COLOR (Default)
    //          - COST_COLOR_GRAD
    stitcher->setSeamFinder(
                cv::makePtr<cv::detail::GraphCutSeamFinder>(
                        cv::detail::GraphCutSeamFinderBase::COST_COLOR
                    )
            );
    // Blender:
    stitcher->setBlender(
                // arg_name (default)
                // try_use_gpu (false), num_bands (5)
                cv::makePtr<cv::detail::MultiBandBlender>(true, 0)
            );
    // Features finder:
    //      - ORB (Default, fast and good enough)
    //      - SIFT (Best matcher, slow as fuck)
    //      - AKAZE (Somewhere in between ORB and SIFT, slow-ish)
    stitcher->setFeaturesFinder(
                cv::ORB::create(700)
            );
    // Interpolation:
    //      - INTER_LINEAR (Default)
    //      - INTER_CUBIC
    //      - INTER_LANCZOS4
    stitcher->setInterpolationFlags(cv::INTER_LANCZOS4);
    // Estimator:
    //      - AffineBasedEstimator (Default)
    //      - HomographyBasedEstimator
    stitcher->setEstimator(
                cv::makePtr<cv::detail::AffineBasedEstimator>()
            );
    // Wave correction:
    //      - Not needed for anime
    //      - Could try with true and setting waveCorrectKind to HORIZ or VERT depending on movement
    stitcher->setWaveCorrection(false);
    // Features matcher:
    //      - AffineBestOf2NearestMatcher (Default)
    stitcher->setFeaturesMatcher(
                // arg_name (default)
                // full_affine (false), try_use_gpu (false), match_conf (0.3f), num_matches_thresh1 (6)
                cv::makePtr<cv::detail::AffineBestOf2NearestMatcher>(false, true, 0.3f, 6)
            );
    // Bundle adjuster
    stitcher->setBundleAdjuster(
                cv::makePtr<cv::detail::BundleAdjusterAffinePartial>()
            );
    // Warper:
    //      - AffineWarper (Default)
    //      - PlaneWarper
    stitcher->setWarper(
                cv::makePtr<cv::AffineWarper>()
            );
    // Exposure compensator:
    //      - NoExposureCompensator (Default)
    //      - GainCompensator
    stitcher->setExposureCompensator(
                cv::makePtr<cv::detail::GainCompensator>()
            );

    // Return the cv::Stitcher object
    return stitcher;
}

std::expected<cv::Mat, cv::Stitcher::Status> ImageStitcher::stitchImages(const std::vector<cv::Mat> &cv_mats) {
    cv::Ptr<cv::Stitcher> stitcher = this->getStitcherPtr();

    cv::Mat stitched_image;
    cv::Stitcher::Status stitcher_status;

    stitcher_status = stitcher->stitch(cv_mats, stitched_image);

    if (stitcher_status != cv::Stitcher::OK) {
        return std::unexpected(stitcher_status);
    }

    // Release stitcher ptr
    stitcher.release();

    return stitched_image;
}

void ImageStitcher::receive_ImageStitcher_start_request(const std::vector<cv::Mat> &cv_mats) {
    // Show progress bar
    emit send_ImageStitcher_show_progress_bar();

    // TODO: Implement batching here
    //       Use QtConcurrent to process multiple batches

    std::expected<cv::Mat, cv::Stitcher::Status> result = this->stitchImages(cv_mats);

    if (result) {
        emit send_ImageStitcher_data(result.value());
    } else {
        emit send_ImageStitcher_status(ImageStitcherStatus::EST_FAIL);
    }
}


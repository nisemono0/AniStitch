#include "base/imagestitcher.hpp"

#include "utils/log.hpp"


ImageStitcher::ImageStitcher(QObject *parent) : QObject(parent) {
    // TODO: Set some stitcher settings or split the pipeline
    this->stitcher = cv::Stitcher::create(cv::Stitcher::SCANS);

    // Set image registration resolution
    this->stitcher->setRegistrationResol(1);
    // Set image resolution for seam estimation
    this->stitcher->setSeamEstimationResol(0.2);
    // Set resolution for final stitch
    this->stitcher->setCompositingResol(cv::Stitcher::ORIG_RESOL);
    // Confidence threshold for images to be part of the same stitch (Default: 1)
    this->stitcher->setPanoConfidenceThresh(0.7);
    // Seam finder:
    //      - GraphCutSeamFinder:
    //          - COST_COLOR (Default)
    //          - COST_COLOR_GRAD
    this->stitcher->setSeamFinder(
                cv::makePtr<cv::detail::GraphCutSeamFinder>(
                        cv::detail::GraphCutSeamFinderBase::COST_COLOR_GRAD
                    )
            );
    // Blender:
    this->stitcher->setBlender(
                // arg_name (default)
                // try_use_gpu (false), num_bands (5)
                cv::makePtr<cv::detail::MultiBandBlender>(true, 2)
            );
    // Features finder:
    //      - ORB (Default)
    //      - SIFT
    //      - AKAZE
    this->stitcher->setFeaturesFinder(
                cv::SIFT::create(2000)
            );
    // Interpolation:
    //      - INTER_LINEAR (Default)
    //      - INTER_CUBIC
    //      - INTER_LANCZOS4
    this->stitcher->setInterpolationFlags(cv::INTER_LANCZOS4);
    // Estimator:
    //      - AffineBasedEstimator (Default)
    //      - HomographyBasedEstimator
    this->stitcher->setEstimator(
                cv::makePtr<cv::detail::AffineBasedEstimator>()
            );
    // Wave correction:
    //      - Not needed for anime
    //      - Could try with true and setting waveCorrectKind to HORIZ or VERT depending on movement
    this->stitcher->setWaveCorrection(false);
    // Features matcher:
    //      - AffineBestOf2NearestMatcher (Default)
    this->stitcher->setFeaturesMatcher(
                // arg_name (default)
                // full_affine (false), try_use_gpu (false), match_conf (0.3f), num_matches_thresh1 (6)
                cv::makePtr<cv::detail::AffineBestOf2NearestMatcher>(false, true, 0.4f, 6)
            );
    // Bundle adjuster
    this->stitcher->setBundleAdjuster(
                cv::makePtr<cv::detail::BundleAdjusterAffinePartial>()
            );
    // Warper:
    //      - AffineWarper (Default)
    //      - PlaneWarper
    this->stitcher->setWarper(
                cv::makePtr<cv::AffineWarper>()
            );
    // Exposure compensator:
    //      - NoExposureCompensator (Default)
    //      - GainCompensator
    this->stitcher->setExposureCompensator(
                cv::makePtr<cv::detail::GainCompensator>()
            );
}

ImageStitcher::~ImageStitcher() {
    this->stitcher.release();
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

    // TODO: Remove those and do it properly
    cv::Mat pano;
    this->stitcher->stitch(cv_mats, pano);
    emit send_ImageStitcher_status(ImageStitcher::Ok);
    emit send_ImageStitcher_data(pano);
}


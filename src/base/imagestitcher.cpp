#include "base/imagestitcher.hpp"

#include "utils/log.hpp"
#include "utils/opencv.hpp"


ImageStitcher::ImageStitcher(QObject *parent) : QObject(parent) {
    this->stitcher = this->getStitcherPtr();
}

ImageStitcher::~ImageStitcher() {
    this->stitcher.release();
}

cv::Ptr<cv::Stitcher> ImageStitcher::getStitcherPtr() {
    stitcher = cv::Stitcher::create(cv::Stitcher::SCANS);

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
    cv::Mat pano_alpha = Utils::Image::getBGRAMat(pano, this->stitcher->resultMask());
    emit send_ImageStitcher_status(ImageStitcher::Ok);
    emit send_ImageStitcher_data(pano_alpha);
}


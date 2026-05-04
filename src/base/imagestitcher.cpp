#include "base/imagestitcher.hpp"

#include "utils/log.hpp"
#include "utils/opencv.hpp"

#include <QtConcurrent>


ImageStitcher::ImageStitcher(QObject *parent) : QObject(parent) {
    // QThreadPool will use optimal max thread count if not set
    this->thread_pool = new QThreadPool(this);
}

ImageStitcher::~ImageStitcher() {
    delete this->thread_pool;
}

cv::Ptr<cv::Stitcher> ImageStitcher::getStitcherPtr() {
    // TODO:
    //      - Split the high level API pipeline into each component
    //      - This way I can also get a stitched image with proper alpha channel
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::SCANS);

    // Set image registration resolution
    stitcher->setRegistrationResol(0.7);
    // Set image resolution for seam estimation
    stitcher->setSeamEstimationResol(0.1);
    // Set resolution for final stitch
    stitcher->setCompositingResol(cv::Stitcher::ORIG_RESOL);
    // Confidence threshold for images to be part of the same stitch (Default: 1)
    stitcher->setPanoConfidenceThresh(1);
    // Seam finder:
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
    stitcher->setFeaturesFinder(
                // cv::ORB::create(700) // Faster, but finds worse matches
                cv::SIFT::create(700) // Slower than ORB but find better matches (~2s slower on 700)
            );
    // Interpolation:
    stitcher->setInterpolationFlags(
                cv::INTER_LANCZOS4
            );
    // Estimator:
    stitcher->setEstimator(
                cv::makePtr<cv::detail::AffineBasedEstimator>()
            );
    // Wave correction:
    stitcher->setWaveCorrection(false);
    // Features matcher:
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
    stitcher->setWarper(
                cv::makePtr<cv::AffineWarper>()
            );
    // Exposure compensator:
    stitcher->setExposureCompensator(
                cv::makePtr<cv::detail::BlocksGainCompensator>()
            );

    // Return the cv::Stitcher object
    return stitcher;
}

std::expected<cv::Mat, cv::Stitcher::Status> ImageStitcher::stitchImages(const std::vector<cv::Mat> &cv_mats) {
    // If cv_mats only has 1 image return it since
    // the stitcher needs 2 images for stitching
    if (cv_mats.size() == 1) {
        return cv_mats[0];
    }

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

std::vector<std::vector<cv::Mat>> ImageStitcher::splitIntoChunks(const std::vector<cv::Mat> &cv_mats) {
    std::vector<std::vector<cv::Mat>> cv_mats_chunks = std::vector<std::vector<cv::Mat>>();

    for (auto chunk : cv_mats | std::views::chunk(ImageStitcher::STITCHER_CHUNK_SIZE)) {
        cv_mats_chunks.emplace_back(chunk.begin(), chunk.end());
    }

    return cv_mats_chunks;
}

void ImageStitcher::receive_ImageStitcher_start_request(const std::vector<cv::Mat> &cv_mats) {
    // Show progress bar
    emit send_ImageStitcher_show_progress_bar();

    int total_images = cv_mats.size();

    std::vector<cv::Mat> image_list = cv_mats;

    // Process until cv_mats has a single image
    while (image_list.size() > 1) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit send_ImageStitcher_status(ImageStitcherStatus::INTERRUPTED);
            return;
        }
        // Split into chunks to process
        std::vector<std::vector<cv::Mat>> cv_mats_chunks = this->splitIntoChunks(image_list);
        // Thread futures
        QList<QFuture<std::expected<cv::Mat, cv::Stitcher::Status>>> stitch_thread_futures;
        for (auto &cv_mat_chunk : cv_mats_chunks) {
            stitch_thread_futures.append(
                        QtConcurrent::run(
                                this->thread_pool,
                                [this, cv_mat_chunk] {
                                    return this->stitchImages(cv_mat_chunk);
                                }
                            )
                    );
        }

        std::vector<cv::Mat> stitched_images = std::vector<cv::Mat>();
        for (auto &stitch_future : stitch_thread_futures) {
            if (QThread::currentThread()->isInterruptionRequested()) {
                emit send_ImageStitcher_status(ImageStitcherStatus::INTERRUPTED);
                return;
            }
            // Wait for a thread to finish before taking the result
            stitch_future.waitForFinished();
            // get stitch result
            auto stitch_result = stitch_future.result();
            // If result is cv::Mat add it to the stitched_images vector
            if (stitch_result) {
                // Add stitched images for reprocessing
                stitched_images.push_back(stitch_result.value());
                // Send progress
                int num_image_stitched = total_images - (image_list.size() - stitched_images.size());
                int progress = (num_image_stitched * 100) / total_images;
                emit send_ImageStitcher_progress(progress);
            // If result is cv::Stitcher::Status process it and emit signal
            } else {
                switch (stitch_result.error()) {
                    case cv::Stitcher::ERR_NEED_MORE_IMGS:
                    {
                        emit send_ImageStitcher_status(ImageStitcherStatus::NEED_MORE_IMGS);
                        return;
                    }
                    case cv::Stitcher::ERR_HOMOGRAPHY_EST_FAIL:
                    {
                        emit send_ImageStitcher_status(ImageStitcherStatus::EST_FAIL);
                        return;
                    }
                    case cv::Stitcher::ERR_CAMERA_PARAMS_ADJUST_FAIL:
                    {
                        emit send_ImageStitcher_status(ImageStitcherStatus::ADJUST_FAIL);
                        return;
                    }
                    case cv::Stitcher::OK: break;
                }
            }
        }

        image_list = stitched_images;
    }

    // image_list should contain the final stitched image
    // If it contains more images it means that the stitch failed somehow
    if (image_list.size() != 1) {
        emit send_ImageStitcher_status(ImageStitcherStatus::NOT_DONE);
        return;
    }

    // Send the last progress since the while loop
    // doesn't count for the last stitched image
    emit send_ImageStitcher_progress(99);
    // image_list[0] is the full stitched image
    emit send_ImageStitcher_data(image_list[0]);
    // Send stitcher status
    emit send_ImageStitcher_status(ImageStitcherStatus::OK);
}


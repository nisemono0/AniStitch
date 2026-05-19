#include "base/imagestitcher.hpp"

#include "utils/log.hpp"
#include "utils/opencv.hpp"
#include "utils/defs.hpp"

#include <QtConcurrent>
#include <QElapsedTimer>
#include <QDateTime>
#include <QUnhandledException>


ImageStitcher::ImageStitcher(QObject *parent) : QObject(parent) {
    // QThreadPool will use optimal max thread count if not set
    this->thread_pool = new QThreadPool(this);
}

ImageStitcher::~ImageStitcher() {
    delete this->thread_pool;
}

void ImageStitcher::startWorker(const std::vector<cv::Mat> &cv_mats, ImageStitcher::ImageStitcherType stitcher_type, const StitcherSettings &stitcher_settings) {
    // Show progress bar
    emit send_ImageStitcher_show_progress_bar();

    int chunk_size = ImageStitcher::DEFAULT_CHUNK_SIZE;

    switch (stitcher_type) {
        case ImageStitcherType::SCAN:
        {
            chunk_size = ImageStitcher::SCAN_CHUNKS_SIZE;
            break;
        }
        case ImageStitcherType::PANORAMA:
        {
            chunk_size = ImageStitcher::PANORAMA_CHUNKS_SIZE;
            break;
        }
        case ImageStitcherType::CUSTOM_SCAN:
        {
            chunk_size = ImageStitcher::SCAN_CHUNKS_SIZE;
            break;
        }
        case ImageStitcherType::CUSTOM_PANORAMA:
        {
            chunk_size = ImageStitcher::PANORAMA_CHUNKS_SIZE;
            break;
        }
    }

    int total_steps = this->getTotalProgressToDo(cv_mats.size(), chunk_size);
    int current_step = 0;

    std::vector<cv::Mat> image_list = cv_mats;

    Log::info(QStringLiteral("Start stitching images: %1").arg(QString::number(cv_mats.size())));

    QElapsedTimer stitcher_timer;
    stitcher_timer.start();

    // Process until cv_mats has a single image
    while (image_list.size() > 1) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit send_ImageStitcher_status(ImageStitcherStatus::INTERRUPTED);
            return;
        }

        Log::info(QStringLiteral("  -> Images left to stitch: %1").arg(QString::number(image_list.size())));

        // Split into chunks to process
        std::vector<std::vector<cv::Mat>> cv_mats_chunks = this->splitIntoChunks(image_list, chunk_size);

        Log::info(QStringLiteral("  -> Chunks to stitch: %1").arg(QString::number(cv_mats_chunks.size())));

        // Thread futures
        QList<QFuture<std::expected<cv::Mat, cv::Stitcher::Status>>> stitch_thread_futures;
        for (auto &cv_mat_chunk : cv_mats_chunks) {
            stitch_thread_futures.append(
                        QtConcurrent::run(
                                this->thread_pool,
                                [this, cv_mat_chunk, stitcher_type, stitcher_settings] {
                                    return this->stitchImages(cv_mat_chunk, stitcher_type, stitcher_settings);
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
                current_step++;
                int progress = (current_step * 100) / total_steps;
                emit send_ImageStitcher_progress(progress);
            // If result is cv::Stitcher::Status process it and emit signal
            } else {
                switch (stitch_result.error()) {
                    case cv::Stitcher::ERR_NEED_MORE_IMGS:
                    {
                        Log::error(QStringLiteral("Error stitching images: NEED_MORE_IMGS"));
                        emit send_ImageStitcher_status(ImageStitcherStatus::NEED_MORE_IMGS);
                        return;
                    }
                    case cv::Stitcher::ERR_HOMOGRAPHY_EST_FAIL:
                    {
                        Log::error(QStringLiteral("Error stitching images: EST_FAIL"));
                        emit send_ImageStitcher_status(ImageStitcherStatus::EST_FAIL);
                        return;
                    }
                    case cv::Stitcher::ERR_CAMERA_PARAMS_ADJUST_FAIL:
                    {
                        Log::error(QStringLiteral("Error stitching images: ADJUST_FAIL"));
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
        Log::error(QStringLiteral("Error stitching images: NOT_DONE"));
        emit send_ImageStitcher_status(ImageStitcherStatus::NOT_DONE);
        return;
    }

    // image_list[0] is the normal full stitched image
    cv::Mat normal_mat = image_list[0];
    cv::Mat parsed_mat = Utils::Image::getBGRAMatFromParsing(normal_mat);
    cv::Mat thresholded_mat = Utils::Image::getBGRAMatFromThreshold(normal_mat);

    QTime time = QTime(0, 0);
    QString sec_done = time.addMSecs(
            stitcher_timer.elapsed()
            ).toString("mm:ss.zz");
    Log::info(QStringLiteral("Stitching done in: %1").arg(sec_done));

    emit send_ImageStitcher_data(normal_mat, parsed_mat, thresholded_mat);
    // Send stitcher status
    emit send_ImageStitcher_status(ImageStitcherStatus::OK);
}

cv::Ptr<cv::Stitcher> ImageStitcher::getScanStitcherPtr() {
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::SCANS);

    // Set image registration resolution (Default: 0.6)
    stitcher->setRegistrationResol(0.7);
    // Set image resolution for seam estimation (Default: 0.1)
    stitcher->setSeamEstimationResol(0.1);
    // Set resolution for final stitch (Default: ORIG_RESOL)
    stitcher->setCompositingResol(cv::Stitcher::ORIG_RESOL);
    // Confidence threshold for images to be part of the same stitch (Default: 1)
    stitcher->setPanoConfidenceThresh(0.7);
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
                cv::SIFT::create(2000)
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
                // arg_name (default)
                // BlocksGainCompensator: bl_width (32), bl_height (32), nr_feeds (1)
                cv::makePtr<cv::detail::BlocksGainCompensator>()
            );

    // Return the cv::Stitcher object
    return stitcher;
}

cv::Ptr<cv::Stitcher> ImageStitcher::getPanoramaStitcherPtr() {
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::PANORAMA);

    // Set image registration resolution (Default: 0.6)
    stitcher->setRegistrationResol(0.7);
    // Set image resolution for seam estimation (Default: 0.1)
    stitcher->setSeamEstimationResol(0.1);
    // Set resolution for final stitch (Default: ORIG_RESOL)
    stitcher->setCompositingResol(cv::Stitcher::ORIG_RESOL);
    // Confidence threshold for images to be part of the same stitch (Default: 1)
    stitcher->setPanoConfidenceThresh(0.7);
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
                cv::SIFT::create(2000)
            );
    // Interpolation:
    stitcher->setInterpolationFlags(
                cv::INTER_LANCZOS4
            );
    // Estimator:
    stitcher->setEstimator(
                cv::makePtr<cv::detail::HomographyBasedEstimator>()
            );
    // Wave correction:
    stitcher->setWaveCorrection(true);
    stitcher->setWaveCorrectKind(cv::detail::WAVE_CORRECT_HORIZ);
    // Features matcher:
    stitcher->setFeaturesMatcher(
                // arg_name (default)
                // try_use_gpu (false), float match_conf (0.3f), num_matches_thresh1 (6), num_matches_thresh2 (6), matches_confindece_thresh = (3.)
                cv::makePtr<cv::detail::BestOf2NearestMatcher>(true, 0.3f, 6, 6, 3.)
            );
    // Bundle adjuster
    stitcher->setBundleAdjuster(
                cv::makePtr<cv::detail::BundleAdjusterRay>()
            );
    // Warper:
    stitcher->setWarper(
                cv::makePtr<cv::PlaneWarper>()
            );
    // Exposure compensator:
    stitcher->setExposureCompensator(
                // arg_name (default)
                // BlocksGainCompensator: bl_width (32), bl_height (32), nr_feeds (1)
                cv::makePtr<cv::detail::BlocksGainCompensator>()
            );

    // Return the cv::Stitcher object
    return stitcher;
}

cv::Ptr<cv::Stitcher> ImageStitcher::getCustomScanStitcherPtr(const StitcherSettings &stitcher_settings) {
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::SCANS);

    // Set image registration resolution (Default: 0.6)
    stitcher->setRegistrationResol(stitcher_settings.registration_res);
    // Set image resolution for seam estimation (Default: 0.1)
    stitcher->setSeamEstimationResol(stitcher_settings.seam_res);
    // Set resolution for final stitch (Default: ORIG_RESOL)
    stitcher->setCompositingResol(cv::Stitcher::ORIG_RESOL);
    // Confidence threshold for images to be part of the same stitch (Default: 1)
    stitcher->setPanoConfidenceThresh(stitcher_settings.confidence_threshold);
    // Seam finder:
    switch (stitcher_settings.seam_finder) {
        case StitcherSettings::SeamFinder::COST_COLOR:
        {
            stitcher->setSeamFinder(
                        cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR)
                    );
            break;

        }
        case StitcherSettings::SeamFinder::COST_COLOR_GRAD:
        {
            stitcher->setSeamFinder(
                        cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR_GRAD)
                    );
            break;
        }
    }
    // Blender:
    stitcher->setBlender(
                // arg_name (default)
                // try_use_gpu (false), num_bands (5)
                cv::makePtr<cv::detail::MultiBandBlender>(true, stitcher_settings.blender_bands)
            );
    // Features finder:
    switch (stitcher_settings.features_finder) {
        case StitcherSettings::SIFT:
        {
            stitcher->setFeaturesFinder(cv::SIFT::create(stitcher_settings.features_finder_number));
            break;

        }
        case StitcherSettings::ORB:
        {
            stitcher->setFeaturesFinder(cv::ORB::create(stitcher_settings.features_finder_number));
            break;
        }
    }
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
    switch (stitcher_settings.exposure_compensator) {
        case StitcherSettings::ExposureCompensator::BLOCKS_GAIN:
        {
            stitcher->setExposureCompensator(
                    cv::makePtr<cv::detail::BlocksGainCompensator>(
                        stitcher_settings.compensator_block_size,
                        stitcher_settings.compensator_block_size,
                        stitcher_settings.compensator_feeds
                        )
                    );
            break;
        }
        case StitcherSettings::ExposureCompensator::BLOCKS_CHANNEL:
        {
            stitcher->setExposureCompensator(
                    cv::makePtr<cv::detail::BlocksChannelsCompensator>(
                        stitcher_settings.compensator_block_size,
                        stitcher_settings.compensator_block_size,
                        stitcher_settings.compensator_feeds
                        )
                    );

        }
    }

    // Return the cv::Stitcher object
    return stitcher;
}

cv::Ptr<cv::Stitcher> ImageStitcher::getCustomPanoramaStitcherPtr(const StitcherSettings &stitcher_settings) {
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(cv::Stitcher::PANORAMA);

    // Set image registration resolution (Default: 0.6)
    stitcher->setRegistrationResol(stitcher_settings.registration_res);
    // Set image resolution for seam estimation (Default: 0.1)
    stitcher->setSeamEstimationResol(stitcher_settings.seam_res);
    // Set resolution for final stitch (Default: ORIG_RESOL)
    stitcher->setCompositingResol(cv::Stitcher::ORIG_RESOL);
    // Confidence threshold for images to be part of the same stitch (Default: 1)
    stitcher->setPanoConfidenceThresh(stitcher_settings.confidence_threshold);
    // Seam finder:
    switch (stitcher_settings.seam_finder) {
        case StitcherSettings::SeamFinder::COST_COLOR:
        {
            stitcher->setSeamFinder(
                        cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR)
                    );
            break;

        }
        case StitcherSettings::SeamFinder::COST_COLOR_GRAD:
        {
            stitcher->setSeamFinder(
                        cv::makePtr<cv::detail::GraphCutSeamFinder>(cv::detail::GraphCutSeamFinderBase::COST_COLOR_GRAD)
                    );
            break;
        }
    }
    // Blender:
    stitcher->setBlender(
                // arg_name (default)
                // try_use_gpu (false), num_bands (5)
                cv::makePtr<cv::detail::MultiBandBlender>(true, stitcher_settings.blender_bands)
            );
    // Features finder:
    switch (stitcher_settings.features_finder) {
        case StitcherSettings::SIFT:
        {
            stitcher->setFeaturesFinder(cv::SIFT::create(stitcher_settings.features_finder_number));
            break;

        }
        case StitcherSettings::ORB:
        {
            stitcher->setFeaturesFinder(cv::ORB::create(stitcher_settings.features_finder_number));
            break;
        }
    }
    // Interpolation:
    stitcher->setInterpolationFlags(
                cv::INTER_LANCZOS4
            );
    // Estimator:
    stitcher->setEstimator(
                cv::makePtr<cv::detail::HomographyBasedEstimator>()
            );
    // Wave correction:
    stitcher->setWaveCorrection(true);
    switch (stitcher_settings.wave_correct) {
        case StitcherSettings::WaveCorrect::HORIZONTAL:
        {
            stitcher->setWaveCorrectKind(cv::detail::WAVE_CORRECT_HORIZ);
            break;
        }
        case StitcherSettings::WaveCorrect::VERTICAL:
        {
            stitcher->setWaveCorrectKind(cv::detail::WAVE_CORRECT_VERT);
            break;
        }
        case StitcherSettings::WaveCorrect::AUTO:
        {
            stitcher->setWaveCorrectKind(cv::detail::WAVE_CORRECT_AUTO);
            break;
        }
    }
    // Features matcher:
    stitcher->setFeaturesMatcher(
                // arg_name (default)
                // try_use_gpu (false), float match_conf (0.3f), num_matches_thresh1 (6), num_matches_thresh2 (6), matches_confindece_thresh = (3.)
                cv::makePtr<cv::detail::BestOf2NearestMatcher>(true, 0.3f, 6, 6, 3.)
            );
    // Bundle adjuster
    stitcher->setBundleAdjuster(
                cv::makePtr<cv::detail::BundleAdjusterRay>()
            );
    // Warper:
    switch (stitcher_settings.warper) {
        case StitcherSettings::PLANE:
        {
            stitcher->setWarper(cv::makePtr<cv::PlaneWarper>());
            break;
        }
        case StitcherSettings::SPHERICAL:
        {
            stitcher->setWarper(cv::makePtr<cv::SphericalWarper>());
            break;
        }
        case StitcherSettings::CYLINDRICAL:
        {
            stitcher->setWarper(cv::makePtr<cv::CylindricalWarper>());
            break;
        }
    }

    // Exposure compensator:
    switch (stitcher_settings.exposure_compensator) {
        case StitcherSettings::ExposureCompensator::BLOCKS_GAIN:
        {
            stitcher->setExposureCompensator(
                    cv::makePtr<cv::detail::BlocksGainCompensator>(
                        stitcher_settings.compensator_block_size,
                        stitcher_settings.compensator_block_size,
                        stitcher_settings.compensator_feeds
                        )
                    );
            break;
        }
        case StitcherSettings::ExposureCompensator::BLOCKS_CHANNEL:
        {
            stitcher->setExposureCompensator(
                    cv::makePtr<cv::detail::BlocksChannelsCompensator>(
                        stitcher_settings.compensator_block_size,
                        stitcher_settings.compensator_block_size,
                        stitcher_settings.compensator_feeds
                        )
                    );

        }
    }

    // Return the cv::Stitcher object
    return stitcher;

}

std::expected<cv::Mat, cv::Stitcher::Status> ImageStitcher::stitchImages(const std::vector<cv::Mat> &cv_mats, ImageStitcher::ImageStitcherType stitcher_type, const StitcherSettings &stitcher_settings) {
    // If cv_mats only has 1 image return it since
    // the stitcher needs 2 images for stitching
    if (cv_mats.size() == 1) {
        return cv_mats[0];
    }

    std::vector<cv::Mat> bgr_mats;

    // cv::Stitcher doesn't like BGRA images
    for (auto &mat : cv_mats) {
        if (mat.channels() == 4) {
            cv::Mat bgr;
            cv::cvtColor(mat, bgr, cv::COLOR_BGRA2BGR);
            bgr_mats.push_back(bgr);
        } else {
            bgr_mats.push_back(mat);
        }
    }

    cv::Ptr<cv::Stitcher> stitcher;
    switch (stitcher_type) {
        case SCAN:
        {
            stitcher = this->getScanStitcherPtr();
            break;
        }
        case PANORAMA:
        {
            stitcher = this->getPanoramaStitcherPtr();
            break;
        }
        case CUSTOM_SCAN:
        {
            stitcher = this->getCustomScanStitcherPtr(stitcher_settings);
            break;
        }
        case CUSTOM_PANORAMA:
        {
            stitcher = this->getCustomPanoramaStitcherPtr(stitcher_settings);
            break;
        }
    }

    cv::Mat stitched_image;
    cv::Stitcher::Status stitcher_status;

    stitcher_status = stitcher->stitch(bgr_mats, stitched_image);

    if (stitcher_status != cv::Stitcher::OK) {
        stitcher.release();
        return std::unexpected(stitcher_status);
    }

    // Release stitcher ptr
    stitcher.release();

    return stitched_image;
}

std::vector<std::vector<cv::Mat>> ImageStitcher::splitIntoChunks(const std::vector<cv::Mat> &cv_mats, int chunk_size) {
    std::vector<std::vector<cv::Mat>> cv_mats_chunks = std::vector<std::vector<cv::Mat>>();

    for (auto chunk : cv_mats | std::views::chunk(chunk_size)) {
        cv_mats_chunks.emplace_back(chunk.begin(), chunk.end());
    }

    return cv_mats_chunks;
}

int ImageStitcher::getTotalProgressToDo(int size, int chunk_size) {
    int total_steps = 1;
    int current_step = std::ceil(static_cast<double>(size) / chunk_size);

    while (current_step > 1) {
        total_steps += current_step;
        current_step = std::ceil(static_cast<double>(current_step) / chunk_size);
    }

    return total_steps;
}

void ImageStitcher::receive_ImageStitcher_start_request(const std::vector<cv::Mat> &cv_mats, ImageStitcher::ImageStitcherType stitcher_type, const StitcherSettings &stitcher_settings) {
    try {
        this->startWorker(cv_mats, stitcher_type, stitcher_settings);
    } catch (const QUnhandledException &ex) {
        try {
            if (ex.exception()) {
                std::rethrow_exception(ex.exception());
            }
        } catch (cv::Exception &ex) {
            Log::error(QStringLiteral("Image stitcher cv::Exception: %1").arg(ex.what()));
        } catch (std::exception &ex) {
            Log::error(QStringLiteral("Image stitcher std::exception: %1").arg(ex.what()));
        }

        emit send_ImageStitcher_status(ImageStitcherStatus::EXCEPTION);
    }
}


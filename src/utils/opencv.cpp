#include "utils/opencv.hpp"
#include "utils/log.hpp"

#include "base/imageitem.hpp"

#include <opencv2/core/ocl.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>


QPixmap Utils::Image::getPixmapFromMat(const cv::Mat &cv_mat) {
    // Return empty pixmap if cv_mat is empty
    if (cv_mat.empty()) {
        return QPixmap();
    }

    QImage image = QImage();
    // Check if cv_mat has 4 channels and load QImage accordingly
    if (cv_mat.channels() == 4) {
        image = QImage(cv_mat.data, cv_mat.cols, cv_mat.rows, static_cast<int>(cv_mat.step), QImage::Format_ARGB32);
    } else {
        image = QImage(cv_mat.data, cv_mat.cols, cv_mat.rows, static_cast<int>(cv_mat.step), QImage::Format_BGR888);
    }

    return QPixmap::fromImage(image);
}

bool Utils::Image::saveMatToPath(const cv::Mat &cv_mat, const QString &file_path) {
    std::string image_path = file_path.toStdString();

    bool image_written = false;
    try {
        image_written = cv::imwrite(file_path.toStdString(), cv_mat);
    } catch (const cv::Exception &cv_exception) {
        Log::error(QStringLiteral("OpenCV imwrite exception: %1").arg(cv_exception.what()));
    }

    return image_written;
}

cv::Mat Utils::Image::getBGRAMat(const cv::Mat &cv_mat, const cv::UMat &image_mask) {
    // Convert cv_mat to BGRA
    cv::Mat bgra_mat;
    cv::cvtColor(cv_mat, bgra_mat, cv::COLOR_BGR2BGRA);
    // Split the BGRA channels
    std::vector<cv::UMat> bgra_mat_channels;
    cv::split(bgra_mat, bgra_mat_channels);
    // Set the Alpha channel from image_mask
    bgra_mat_channels[3] = image_mask;
    // Merge the channels back into bgra_mat
    cv::merge(bgra_mat_channels, bgra_mat);

    return bgra_mat;
}

void Utils::OpenCV::enableOpenCL() {
    if (cv::ocl::haveOpenCL()) {
        Log::info(QStringLiteral("OpenCL is supported, trying to enable it"));

        cv::ocl::setUseOpenCL(true);

        if (cv::ocl::useOpenCL()) {
            Log::info(QStringLiteral("  -> OpenCL is being used"));
        } else {
            Log::warning(QStringLiteral("  -> OpenCL is not being used"));
        }
    } else {
        Log::warning(QStringLiteral("OpenCL not supported, please install an OpenCL runtime"));
    }
}

std::vector<cv::UMat> Utils::OpenCV::getUMatsFromMats(const std::vector<cv::Mat> &cv_mats) {
    std::vector<cv::UMat> cv_umats = std::vector<cv::UMat>();

    for (auto &mat : cv_mats) {
        cv_umats.push_back(
                mat.getUMat(cv::ACCESS_READ)
                );
    }

    return cv_umats;
}


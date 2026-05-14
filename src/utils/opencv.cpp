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

cv::Mat Utils::Image::getBGRAMatFromMask(const cv::Mat &cv_mat, const cv::UMat &image_mask) {
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

cv::Mat Utils::Image::getBGRAMatFromThreshold(const cv::Mat &cv_mat) {
    cv::Mat gray_mat;
    cv::UMat alpha_mask;

    // Convert the cv_mat to gray for thresholding
    cv::cvtColor(cv_mat, gray_mat, cv::COLOR_BGR2GRAY);

    // Threshold the pure black color
    cv::threshold(gray_mat, alpha_mask, 0, 255, cv::THRESH_BINARY);

    // Copy and convert the original image to BGRA then
    // split it, add alpha channel from above, merge it
    cv::Mat alpha_mat;
    cv::cvtColor(cv_mat, alpha_mat, cv::COLOR_BGR2BGRA);

    std::vector<cv::UMat> alpha_mat_channels;
    cv::split(alpha_mat, alpha_mat_channels);
    alpha_mat_channels[3] = alpha_mask;

    cv::merge(alpha_mat_channels, alpha_mat);

    return alpha_mat;
}

cv::Mat Utils::Image::getBGRAMatFromParsing(const cv::Mat &cv_mat) {
    // Gray image and it's corresponding alpha mask
    cv::Mat gray_mat;
    cv::Mat alpha_mask;

    // Convert to gray
    cv::cvtColor(cv_mat, gray_mat, cv::COLOR_BGR2GRAY);
    // Convert the alpha mask to gray and fill it with 255
    cv::cvtColor(cv_mat, alpha_mask, cv::COLOR_BGR2GRAY);
    alpha_mask.setTo(255);

    int rows = gray_mat.rows;
    int cols = gray_mat.cols;

    // Compare each pixel from the outside in
    // and stop at first non 0 pixel found
    // this could've be done better not gonna lie lol
    // Left -> Right; Right -> Left
    for (int y = 0; y < rows; y++) {
        uchar *gray_ptr = gray_mat.ptr<uchar>(y);
        uchar *mask_ptr = alpha_mask.ptr<uchar>(y);
        for (int x = 0; x < cols; x++) {
            if (gray_ptr[x] == 0) {
                mask_ptr[x] = 0;
            } else {
                break;
            }
        }
        for (int x = cols - 1; x >= 0; x--) {
            if (gray_ptr[x] == 0) {
                mask_ptr[x] = 0;
            } else {
                break;
            }
        }
    }
    // Top -> Bottom; Bottom -> Top
    for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
            uchar *gray_ptr = gray_mat.ptr<uchar>(y);
            uchar *mask_ptr = alpha_mask.ptr<uchar>(y);
            if (gray_ptr[x] == 0) {
                mask_ptr[x] = 0;
            } else {
                break;
            }
        }
        for (int y = rows - 1; y >= 0; y--) {
            uchar *gray_ptr = gray_mat.ptr<uchar>(y);
            uchar *mask_ptr = alpha_mask.ptr<uchar>(y);
            if (gray_ptr[x] == 0) {
                mask_ptr[x] = 0;
            } else {
                break;
            }
        }
    }

    // Copy and convert the original image to BGRA then
    // split it, add alpha channel from above, merge it
    cv::Mat alpha_mat;
    cv::cvtColor(cv_mat, alpha_mat, cv::COLOR_BGR2BGRA);

    std::vector<cv::UMat> alpha_mat_channels;
    cv::split(alpha_mat, alpha_mat_channels);
    alpha_mat_channels[3] = alpha_mask.getUMat(cv::ACCESS_READ);

    // Merge the channels into its final image
    cv::merge(alpha_mat_channels, alpha_mat);

    return alpha_mat;
}

cv::Mat Utils::Image::cropMat(const cv::Mat &cv_mat, int top_px, int right_px, int bottom_px, int left_px) {
    int new_width = cv_mat.cols - left_px - right_px;
    int new_height = cv_mat.rows - top_px - bottom_px;

    // if the crop is bigger than the image retur the original image
    if (new_width <= 0 || new_height <= 0) {
        return cv_mat;
    }

    cv::Rect crop_roi = cv::Rect(
                left_px, top_px,
                new_width, new_height
            );

    return cv_mat(crop_roi);
}

void Utils::OpenCV::disableOpenCL() {
    if (cv::ocl::haveOpenCL()) {
        Log::info(QStringLiteral("OpenCL is supported, trying to disable it"));

        cv::ocl::setUseOpenCL(false);

        if (cv::ocl::useOpenCL()) {
            Log::warning(QStringLiteral("  -> OpenCL is enabled"));
        } else {
            Log::info(QStringLiteral("  -> OpenCL is disabled"));
        }
    } else {
        Log::warning(QStringLiteral("OpenCL not supported"));
    }
}


#pragma once

#include <QString>
#include <QPixmap>
#include <QList>

#include <opencv2/core.hpp>


namespace Utils::Image {
    // Returns a QPixmap from a cv::Mat
    QPixmap getPixmapFromMat(const cv::Mat &cv_mat);
    // Save cv::Mat to file_path
    bool saveMatToPath(const cv::Mat &cv_mat, const QString &file_path);
    // Return a cv::Mat with alpha channel (BGRA) from input cv_mat and image_mask
    cv::Mat getBGRAMat(const cv::Mat &cv_mat, const cv::UMat &image_mask);
};

namespace Utils::OpenCV {
    // Try to enable OpenCL if available
    void enableOpenCL();
    // Return a std::vector of cv::UMat from a std::vector of cv::Mat
    // to be used with OpenCL
    std::vector<cv::UMat> getUMatsFromMats(const std::vector<cv::Mat> &cv_mats);
}


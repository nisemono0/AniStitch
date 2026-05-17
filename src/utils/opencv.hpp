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
    // Return a cv::Mat with alpha channel (BGRA) from an input
    // cv_mat and its corresponding alpha channel image_mask
    cv::Mat getBGRAMatFromMask(const cv::Mat &cv_mat, const cv::UMat &image_mask);
    // Return a cv::Mat with alpha channel (BGRA) for an input
    // cv_mat; Binary thresholding (0, 255) is used to make the alpha channel
    // Can sometimes fail to make a proper mask; if other pure black pixels exist
    // they'll be used too even if they're in the middle of the image
    cv::Mat getBGRAMatFromThreshold(const cv::Mat &cv_mat);
    // Return a cv::Mat with alpha channel (BGRA) from an input
    // cv_mat by looping from the outside in over the image on all directions;
    // Can sometimes fail to make a proper alpha mask; If the
    // cornsers of the image are pure black, they'll be used to
    // make the mask; Somewhat fixed the problem of using thresholding
    cv::Mat getBGRAMatFromParsing(const cv::Mat &cv_mat);
    // Crop a cv_mat a number of top_px/right_px/bottom_px/left_px pixels from top/right/bottom/left
    cv::Mat cropMat(const cv::Mat &cv_mat, int top_px, int right_px, int bottom_px, int left_px);
    // Crop the polygon from polygon_points from the cv_mat
    cv::Mat cropMat(const cv::Mat &cv_mat, const std::vector<cv::Point> &polygon_points);
};

namespace Utils::OpenCV {
    // Disable OpenCL
    void disableOpenCL();
}


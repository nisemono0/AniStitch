#pragma once

#include <opencv2/core.hpp>

#include <QMetaType>


Q_DECLARE_METATYPE(cv::Mat);
Q_DECLARE_METATYPE(std::vector<cv::Mat>);


#pragma once

#include <opencv2/core.hpp>

#include <QMetaType>

struct StitcherSettings {
    enum WaveCorrect { HORIZONTAL, VERTICAL, AUTO };
    enum Warper { PLANE, SPHERICAL, CYLINDRICAL };
    enum ExposureCompensator { BLOCKS_GAIN, BLOCKS_CHANNEL };

    float registration_res = 0.7;
    float seam_res = 0.1;
    float confidence_threshold = 0.7;
    int blender_bands = 0;
    WaveCorrect wave_correct = WaveCorrect::HORIZONTAL;
    Warper warper = Warper::PLANE;
    ExposureCompensator exposure_compensator = ExposureCompensator::BLOCKS_CHANNEL;
    int compensator_block_size = 32;
    int compensator_feeds = 1;
};

Q_DECLARE_METATYPE(cv::Mat);
Q_DECLARE_METATYPE(std::vector<cv::Mat>);
Q_DECLARE_METATYPE(StitcherSettings);


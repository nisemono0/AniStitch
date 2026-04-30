#pragma once

#include <opencv2/core.hpp>

#include <QString>
#include <QPixmap>


// Stored in ImageListView
class ImageItem {
public:
    QString name;
    QPixmap pixmap;
    cv::Mat cvmat;
    bool has_image;

    ImageItem();
    ImageItem(
            const QString &name,
            const QPixmap &pixmap,
            const cv::Mat &cvmat,
            const bool &has_image
         );
};


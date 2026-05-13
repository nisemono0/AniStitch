#pragma once

#include <opencv2/core.hpp>

#include <QString>
#include <QPixmap>


// Stored in ImageListView
class ImageItem {
public:
    QString display_name;
    QString name;
    QPixmap pixmap;
    cv::Mat cvmat;

    ImageItem();
    ImageItem(
            const QString &display_name,
            const QString &name,
            const QPixmap &pixmap,
            const cv::Mat &cvmat
         );
};


#include "base/imageitem.hpp"


ImageItem::ImageItem() {
    this->display_name = QStringLiteral("Empty");
    this->name = QStringLiteral("Empty");
    this->pixmap = QPixmap();
    this->cvmat = cv::Mat();
}

ImageItem::ImageItem(
        const QString &display_name,
        const QString &name,
        const QPixmap &pixmap,
        const cv::Mat &cvmat
    ) :
        display_name(display_name),
        name(name),
        pixmap(pixmap),
        cvmat(cvmat) {}


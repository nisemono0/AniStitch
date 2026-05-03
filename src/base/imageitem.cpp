#include "base/imageitem.hpp"


ImageItem::ImageItem() {
    this->name = QStringLiteral("Empty");
    this->pixmap = QPixmap();
    this->cvmat = cv::Mat();
}

ImageItem::ImageItem(
        const QString &name,
        const QPixmap &pixmap,
        const cv::Mat &cvmat
    ) :
        name(name),
        pixmap(pixmap),
        cvmat(cvmat) {}

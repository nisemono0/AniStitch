#include "base/imageitem.hpp"


ImageItem::ImageItem() {
    this->name = QStringLiteral("Empty");
    this->pixmap = QPixmap();
    this->cvmat = cv::Mat();
    this->has_image = false;
}

ImageItem::ImageItem(
        const QString &name,
        const QPixmap &pixmap,
        const cv::Mat &cvmat,
        const bool &has_image
    ) :
        name(name),
        pixmap(pixmap),
        cvmat(cvmat),
        has_image(has_image) {}


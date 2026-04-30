#include "app/imagegraphicsscene.hpp"

#include "utils/str.hpp"

#include <QUrl>


ImageGraphicsScene::ImageGraphicsScene(QObject *parent) : QGraphicsScene(parent) {
    this->scene_pixmap_item = new QGraphicsPixmapItem(QPixmap());
    this->scene_pixmap_item->setTransformationMode(Qt::SmoothTransformation);
    this->scene_pixmap_item->setVisible(true);

    this->addItem(this->scene_pixmap_item);
}

ImageGraphicsScene::~ImageGraphicsScene() {
    delete this->scene_pixmap_item;
}

void ImageGraphicsScene::setPixmap(const QPixmap &pixmap) {
    this->scene_pixmap_item->setPixmap(pixmap);
    this->setSceneRect(this->scene_pixmap_item->boundingRect());
}

void ImageGraphicsScene::clearScene() {
    this->scene_pixmap_item->setPixmap(QPixmap());
}

QRectF ImageGraphicsScene::getBoundingRect() {
    return this->scene_pixmap_item->boundingRect();
}

void ImageGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void ImageGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void ImageGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event) {
    const QMimeData *mime_data = event->mimeData();

    if (mime_data->hasUrls()) {
        QStringList file_paths = QStringList();

        // Get local file paths from mime_data urls
        for (auto &url : mime_data->urls()) {
            QString local_url = url.toLocalFile();
            if (!Utils::String::isNullOrEmpty(local_url)) {
                file_paths.append(local_url);
            }
        }

        emit send_ImageGraphicsScene_drag_drop_file_paths(file_paths);
    }

    event->acceptProposedAction();
}


#include "app/stitchedimagescene.hpp"


StitchedImageScene::StitchedImageScene(QObject *parent) : QGraphicsScene(parent) {
    this->scene_pixmap_item = new QGraphicsPixmapItem(QPixmap());
    this->scene_pixmap_item->setTransformationMode(Qt::SmoothTransformation);
    this->scene_pixmap_item->setVisible(true);

    this->addItem(this->scene_pixmap_item);
}

StitchedImageScene::~StitchedImageScene() {
    delete this->scene_pixmap_item;
}

void StitchedImageScene::setPixmap(const QPixmap &pixmap) {
    this->scene_pixmap_item->setPixmap(pixmap);
    this->setSceneRect(this->scene_pixmap_item->boundingRect());
}

void StitchedImageScene::clearScene() {
    this->scene_pixmap_item->setPixmap(QPixmap());
}

QRectF StitchedImageScene::getBoundingRect() {
    return this->scene_pixmap_item->boundingRect();
}


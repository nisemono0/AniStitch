#include "app/displayimagescene.hpp"


DisplayImageScene::DisplayImageScene(QObject *parent) : QGraphicsScene(parent) {
    this->scene_pixmap_item = new QGraphicsPixmapItem(QPixmap());
    this->scene_pixmap_item->setTransformationMode(Qt::SmoothTransformation);
    this->scene_pixmap_item->setVisible(true);

    this->addItem(this->scene_pixmap_item);
}

DisplayImageScene::~DisplayImageScene() {
    delete this->scene_pixmap_item;
}

void DisplayImageScene::setPixmap(const QPixmap &pixmap) {
    this->scene_pixmap_item->setPixmap(pixmap);
    this->setSceneRect(this->scene_pixmap_item->boundingRect());
}

void DisplayImageScene::clearScene() {
    this->scene_pixmap_item->setPixmap(QPixmap());
}

QRectF DisplayImageScene::getBoundingRect() {
    return this->scene_pixmap_item->boundingRect();
}


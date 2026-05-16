#include "app/stitchedimageview.hpp"


StitchedImageView::StitchedImageView(QWidget *parent) : QGraphicsView(parent) {
    this->image_scene = new StitchedImageScene(this);
    this->setScene(this->image_scene);
}

StitchedImageView::~StitchedImageView() {
    delete this->image_scene;
}

void StitchedImageView::displayPixmap(const QPixmap &pixmap) {
    this->image_scene->setPixmap(pixmap);
    this->fitImageScene();
}

void StitchedImageView::clearScene() {
    this->image_scene->clearScene();
}

void StitchedImageView::fitImageScene() {
    QRectF padded_bounding_rect = this->image_scene->getBoundingRect();
    padded_bounding_rect.adjust(
            -StitchedImageView::ScenePadding, -StitchedImageView::ScenePadding,
            StitchedImageView::ScenePadding, StitchedImageView::ScenePadding
            );

    this->fitInView(padded_bounding_rect, Qt::KeepAspectRatio);
}

void StitchedImageView::resizeEvent(QResizeEvent *event) {
    this->fitImageScene();
    QGraphicsView::resizeEvent(event);
}


#include "app/displayimageview.hpp"


DisplayImageView::DisplayImageView(QWidget *parent) : QGraphicsView(parent) {
    this->image_scene = new DisplayImageScene(this);
    this->setScene(this->image_scene);
}

DisplayImageView::~DisplayImageView() {
    delete this->image_scene;
}

void DisplayImageView::displayPixmap(const QPixmap &pixmap) {
    this->image_scene->setPixmap(pixmap);
    this->fitImageScene();
}

void DisplayImageView::clearScene() {
    this->image_scene->clearScene();
}

void DisplayImageView::fitImageScene() {
    QRectF padded_bounding_rect = this->image_scene->getBoundingRect();
    padded_bounding_rect.adjust(
            -DisplayImageView::ScenePadding, -DisplayImageView::ScenePadding,
            DisplayImageView::ScenePadding, DisplayImageView::ScenePadding
            );

    this->fitInView(padded_bounding_rect, Qt::KeepAspectRatio);
}

void DisplayImageView::resizeEvent(QResizeEvent *event) {
    this->fitImageScene();
    QGraphicsView::resizeEvent(event);
}


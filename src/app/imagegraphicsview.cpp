#include "app/imagegraphicsview.hpp"


ImageGraphicsView::ImageGraphicsView(QWidget *parent) : QGraphicsView(parent) {
    this->image_scene = new ImageGraphicsScene(this);
    this->setScene(this->image_scene);
}

ImageGraphicsView::~ImageGraphicsView() {
    delete this->image_scene;
}

ImageGraphicsScene* ImageGraphicsView::getGraphicsScene() {
    return this->image_scene;
}

void ImageGraphicsView::fitImageScene() {
    QRectF padded_bounding_rect = this->image_scene->getBoundingRect();
    padded_bounding_rect.adjust(
            -ImageGraphicsView::ScenePadding, -ImageGraphicsView::ScenePadding,
            ImageGraphicsView::ScenePadding, ImageGraphicsView::ScenePadding
            );

    this->fitInView(padded_bounding_rect, Qt::KeepAspectRatio);
}

void ImageGraphicsView::resizeEvent(QResizeEvent *event) {
    this->fitImageScene();
    QGraphicsView::resizeEvent(event);
}

void ImageGraphicsView::receive_show_pixmap_request(const QPixmap &pixmap) {
    this->image_scene->setPixmap(pixmap);
    this->fitImageScene();
}

void ImageGraphicsView::receive_clear_scene_request() {
    this->image_scene->clearScene();
}

void ImageGraphicsView::receive_enable_selection_request() {
    this->image_scene->setSelectionEnabled(true);
    this->setCursor(Qt::CrossCursor);
}

void ImageGraphicsView::receive_disable_selection_request() {
    this->image_scene->setSelectionEnabled(false);
    this->image_scene->resetSelection();
    this->setCursor(Qt::ArrowCursor);
}

void ImageGraphicsView::receive_reset_selection_request() {
    this->image_scene->resetSelection();
}

void ImageGraphicsView::receive_send_selection_request() {
    emit send_ImageGraphicsView_selection(this->image_scene->getSelectionPoints());
}


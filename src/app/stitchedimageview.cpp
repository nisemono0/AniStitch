#include "app/stitchedimageview.hpp"


StitchedImageView::StitchedImageView(QWidget *parent) : QGraphicsView(parent) {
    this->setupBrushes();

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

void StitchedImageView::setupBrushes() {
    // Current brush index should start at 0 (Default one)
    this->current_brush_index = 0;

    // Default brush
    this->brushes_list.append(Qt::NoBrush);

    // Checkered brush
    QPixmap tile_pixmap = QPixmap(StitchedImageView::TileSize * 2, StitchedImageView::TileSize * 2);
    tile_pixmap.fill(Qt::darkGray);

    QPainter tile_painter = QPainter(&tile_pixmap);
    tile_painter.fillRect(0, 0, StitchedImageView::TileSize, StitchedImageView::TileSize, Qt::gray);
    tile_painter.fillRect(StitchedImageView::TileSize, StitchedImageView::TileSize, StitchedImageView::TileSize, StitchedImageView::TileSize, Qt::gray);
    this->brushes_list.append(tile_pixmap);

    // Plain color magenta
    this->brushes_list.append(Qt::magenta);
}

void StitchedImageView::updateBackgroundBrush(BrushDirection brush_direction) {
    switch (brush_direction) {
        case NEXT:
        {
            this->current_brush_index = (this->current_brush_index + 1) % this->brushes_list.size();
            this->setBackgroundBrush(this->brushes_list[this->current_brush_index]);
            break;
        }
        case PREV:
        {
            this->current_brush_index = (this->current_brush_index - 1 + this->brushes_list.size()) % this->brushes_list.size();
            this->setBackgroundBrush(this->brushes_list[this->current_brush_index]);
            break;
        }
        default:
          break;
    }
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

void StitchedImageView::mousePressEvent(QMouseEvent *event) {
    switch (event->button()) {
        case Qt::LeftButton:
        {
            this->updateBackgroundBrush(BrushDirection::NEXT);
            break;
        }
        case Qt::RightButton:
        {
            this->updateBackgroundBrush(BrushDirection::PREV);
            break;
        }
        default:
            break;
    }
    QGraphicsView::mousePressEvent(event);
}


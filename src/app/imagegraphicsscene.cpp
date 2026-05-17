#include "app/imagegraphicsscene.hpp"

#include "utils/str.hpp"

#include <QUrl>
#include <QPainter>
#include <QBrush>


ImageGraphicsScene::ImageGraphicsScene(QObject *parent) : QGraphicsScene(parent) {
    this->scene_pixmap_item = new QGraphicsPixmapItem(QPixmap());
    this->scene_pixmap_item->setTransformationMode(Qt::SmoothTransformation);
    this->scene_pixmap_item->setVisible(true);

    this->scene_selection_item = new QGraphicsPixmapItem(QPixmap());
    this->scene_selection_item->setTransformationMode(Qt::SmoothTransformation);
    this->scene_selection_item->setVisible(true);

    this->addItem(this->scene_pixmap_item);
    this->addItem(this->scene_selection_item);

    this->setSelectionEnabled(false);
}

ImageGraphicsScene::~ImageGraphicsScene() {
    delete this->scene_selection_item;
    delete this->scene_pixmap_item;
}

void ImageGraphicsScene::setPixmap(const QPixmap &pixmap) {
    this->scene_pixmap_item->setPixmap(pixmap);

    QPixmap overlay_pixmap = QPixmap(pixmap.size());
    overlay_pixmap.fill(Qt::transparent);
    this->scene_selection_item->setPixmap(overlay_pixmap);

    // Don't clear the previous selection when changing
    // the shown image and selection is enabled otherwise
    // clear the selection polygon points
    if (this->selection_enabled) {
        this->drawPolygonSelection();
    } else {
        this->selection_polygon.clear();
    }

    this->setSceneRect(this->scene_pixmap_item->boundingRect());
}

void ImageGraphicsScene::clearScene() {
    this->scene_pixmap_item->setPixmap(QPixmap());
    this->scene_selection_item->setPixmap(QPixmap());
    this->selection_polygon.clear();
}

QRectF ImageGraphicsScene::getBoundingRect() {
    return this->scene_pixmap_item->boundingRect();
}

void ImageGraphicsScene::setSelectionEnabled(bool enabled) {
    this->selection_enabled = enabled;
}

void ImageGraphicsScene::resetSelection() {
    QPixmap overlay_pixmap = QPixmap(this->scene_pixmap_item->pixmap().size());
    overlay_pixmap.fill(Qt::transparent);

    this->scene_selection_item->setPixmap(overlay_pixmap);

    this->selection_polygon.clear();
}

std::optional<std::vector<cv::Point>> ImageGraphicsScene::getSelectionPoints() {
    // Return nullopt of less than 3 points are set
    if (this->selection_polygon.size() < 4) {
        return std::nullopt;
    }

    std::vector<cv::Point> polygon_points = std::vector<cv::Point>();
    for (auto &point : this->selection_polygon) {
        polygon_points.push_back(cv::Point(point.x(), point.y()));
    }

    return polygon_points;
}

void ImageGraphicsScene::appendPolygonPoint(const QPoint &polygon_point) {
    // Do nothing if there's not selection pixmap
    if (this->scene_selection_item->pixmap().isNull()) {
        return;
    }

    this->selection_polygon.append(polygon_point);
}

void ImageGraphicsScene::removeLastPolygonPoint() {
    // Don't remove if already empty
    if (this->selection_polygon.isEmpty()) {
        return;
    }
    this->selection_polygon.removeLast();
}

void ImageGraphicsScene::drawPolygonSelection() {
    QPixmap overlay_pixmap = this->scene_selection_item->pixmap();
    // If the overlay_pixmap is null then do nothing
    if (overlay_pixmap.isNull()) {
        return;
    }
    // Clear the overlay_pixmap before drawing on it
    overlay_pixmap.fill(Qt::transparent);

    // Create a painter on the overlay_pixmap and enable Antialiasing
    QPainter selection_painter = QPainter(&overlay_pixmap);
    selection_painter.setRenderHint(QPainter::Antialiasing, true);

    // Only draw the polygon if there are at least 4 points set
    if (this->selection_polygon.size() > 3) {
        QPen selection_pen = QPen(
                Qt::white, ImageGraphicsScene::LINE_WIDTH, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin
                );
        selection_pen.setCosmetic(true);
        selection_painter.setPen(selection_pen);

        QBrush selection_brush = QBrush(QColor(
                    ImageGraphicsScene::SELECTION_RED,
                    ImageGraphicsScene::SELECTION_GREEN,
                    ImageGraphicsScene::SELECTION_BLUE,
                    ImageGraphicsScene::SELECTION_ALPHA
                    ), Qt::SolidPattern);
        selection_painter.setBrush(selection_brush);
        selection_painter.drawPolygon(this->selection_polygon);
    }

    // Draw the points of the polygon
    QBrush points_brush = QBrush(Qt::white, Qt::SolidPattern);
    selection_painter.setBrush(points_brush);
    selection_painter.setPen(Qt::NoPen);
    for (auto &point : this->selection_polygon) {
        selection_painter.drawEllipse(point, ImageGraphicsScene::POINT_RADIUS, ImageGraphicsScene::POINT_RADIUS);
    }

    this->scene_selection_item->setPixmap(overlay_pixmap);
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

void ImageGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) {
    if (mouseEvent->button() == Qt::LeftButton && this->selection_enabled) {
        // Add polygon point on LMB click
        // Mouse position within the scene
        QPoint mouse_pos = mouseEvent->scenePos().toPoint();
        // Force the clicks outside the image inside it
        // Might need to use item boundingRect instead of sceneRect
        // So far it seems to work with this
        int max_x = this->sceneRect().width();
        int max_y = this->sceneRect().height();
        int bound_x = qBound(0, mouse_pos.x(), max_x);
        int bound_y = qBound(0, mouse_pos.y(), max_y);
        // Make a QPoint with the bounded values and add to selection_polygon
        QPoint bound_mouse_pos = QPoint(bound_x, bound_y);
        this->appendPolygonPoint(bound_mouse_pos);
        this->drawPolygonSelection();
    } else if (mouseEvent->button() == Qt::RightButton && this->selection_enabled) {
        // Remove last polygon point on RMB clik
        this->removeLastPolygonPoint();
        this->drawPolygonSelection();
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}


#pragma once

#include <opencv2/core.hpp>

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>


class ImageGraphicsScene : public QGraphicsScene {
    Q_OBJECT;
public:
    explicit ImageGraphicsScene(QObject *parent = nullptr);
    ~ImageGraphicsScene();

    // Set QPixmap to show on the scene
    void setPixmap(const QPixmap &pixmap);
    // Clears the scene
    void clearScene();
    // Return the bounding rect of the scene
    QRectF getBoundingRect();
    // Enable making selections on the scene
    void setSelectionEnabled(bool enabled);
    // Reset the selection set points and drawings
    void resetSelection();
    // Return and std::optional with the current selection points or nullopt
    std::optional<std::vector<cv::Point>> getSelectionPoints();

private:
    // Selection color
    static const int SELECTION_RED   = 255;
    static const int SELECTION_GREEN = 0;
    static const int SELECTION_BLUE  = 0;
    static const int SELECTION_ALPHA = 50;
    // Line width of the polygon
    static const int LINE_WIDTH      = 2;
    // Polygon points radius
    static const int POINT_RADIUS    = 5;

    QGraphicsPixmapItem *scene_pixmap_item;
    QGraphicsPixmapItem *scene_selection_item;

    bool selection_enabled;
    QPolygon selection_polygon;

    // Add polygon point if an image is present
    void appendPolygonPoint(const QPoint &polygon_point);
    // Remove the last added polygon_point
    void removeLastPolygonPoint();
    // Draw a polygon based on selection_polygon points
    void drawPolygonSelection();

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

signals:
    // Send the drag and dropped url_list
    void send_ImageGraphicsScene_drag_drop_file_paths(const QStringList &file_paths);

};


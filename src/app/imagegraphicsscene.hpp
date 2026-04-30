#pragma once

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneDragDropEvent>
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

private:
    QGraphicsPixmapItem *scene_pixmap_item;

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

signals:
    // Send the drag and dropped url_list
    void send_ImageGraphicsScene_drag_drop_file_paths(const QStringList &file_paths);

};


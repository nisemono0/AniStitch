#pragma once

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>


class StitchedImageScene : public QGraphicsScene {
    Q_OBJECT;
public:
    explicit StitchedImageScene(QObject *parent = nullptr);
    ~StitchedImageScene();

    // Set QPixmap to show on scene
    void setPixmap(const QPixmap &pixmap);
    // Clear the scene
    void clearScene();
    // Return the bounding rect of the scene
    QRectF getBoundingRect();

private:
    QGraphicsPixmapItem *scene_pixmap_item;

};


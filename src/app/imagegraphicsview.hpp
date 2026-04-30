#pragma once

#include "app/imagegraphicsscene.hpp"

#include <QWidget>
#include <QGraphicsView>
#include <QResizeEvent>


class ImageGraphicsView : public QGraphicsView {
    Q_OBJECT;
public:
    explicit ImageGraphicsView(QWidget *parent = nullptr);
    ~ImageGraphicsView() override;

    // Return the views ImageGraphicsScene
    ImageGraphicsScene* getGraphicsScene();

private:
    ImageGraphicsScene *image_scene;
    // ScenePadding to use when fitting the scene
    const int ScenePadding = 20;

    // Fit image_scene in view keeping aspect ratio and adding ScenePadding on all sides
    void fitImageScene();

protected:
    void resizeEvent(QResizeEvent *event) override;

signals:

public slots:
    void receive_show_pixmap_request(const QPixmap &pixmap);
    void receive_clear_scene_request();

private slots:
};


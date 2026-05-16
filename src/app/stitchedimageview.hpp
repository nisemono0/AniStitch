#pragma once

#include "app/stitchedimagescene.hpp"

#include <QWidget>
#include <QGraphicsView>
#include <QResizeEvent>


class StitchedImageView : public QGraphicsView {
    Q_OBJECT;
public:
    explicit StitchedImageView(QWidget *parent = nullptr);
    ~StitchedImageView() override;

    // Display pixmap on the scene
    void displayPixmap(const QPixmap &pixmap);
    // Clears the scene
    void clearScene();

private:
    StitchedImageScene *image_scene;
    // ScenePadding to use when fitting the scene
    const int ScenePadding = 5;
    // Fit image_scene in view keeping aspect ratio and adding padding on all sides
    void fitImageScene();

protected:
    void resizeEvent(QResizeEvent *event) override;

};


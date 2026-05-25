#pragma once

#include "app/stitchedimagescene.hpp"

#include <QWidget>
#include <QGraphicsView>
#include <QResizeEvent>
#include <QMouseEvent>


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
    // Enum to set next/prev background brush
    enum BrushDirection { NEXT, PREV };
    // ScenePadding to use when fitting the scene
    const int ScenePadding = 5;
    // Checkered background tile size
    const int TileSize = 16;

    // Graphics scene
    StitchedImageScene *image_scene;

    // List of brushes to paint the background
    // and the index of the current selected brush
    int current_brush_index;
    QList<QBrush> brushes_list;
    // Setup different brushes:
    // - NoBrush
    // - Checkered
    // - Magenta
    void setupBrushes();
    // Update background with next/prev brush
    void updateBackgroundBrush(BrushDirection brush_direction);

    // Fit image_scene in view keeping aspect ratio and adding padding on all sides
    void fitImageScene();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

};


#pragma once

#include "app/displayimagescene.hpp"

#include <QWidget>
#include <QGraphicsView>
#include <QResizeEvent>


class DisplayImageView : public QGraphicsView {
    Q_OBJECT;
public:
    explicit DisplayImageView(QWidget *parent = nullptr);
    ~DisplayImageView() override;

    // Display pixmap on the scene
    void displayPixmap(const QPixmap &pixmap);
    // Clears the scene
    void clearScene();

private:
    DisplayImageScene *image_scene;
    // ScenePadding to use when fitting the scene
    const int ScenePadding = 10;
    // Fit image_scene in view keeping aspect ratio and adding padding on all sides
    void fitImageScene();

protected:
    void resizeEvent(QResizeEvent *event) override;

};


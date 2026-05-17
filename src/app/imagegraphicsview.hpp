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
    void send_ImageGraphicsView_selection(const std::optional<std::vector<cv::Point>> &polygon_points);

public slots:
    // Show given pixmap on the scene
    void receive_show_pixmap_request(const QPixmap &pixmap);
    void receive_clear_scene_request();

    // Enable / Disable selection
    void receive_enable_selection_request();
    void receive_disable_selection_request();

    // Reset current selection coords
    void receive_reset_selection_request();

    // Retrieve the current selection's coords and
    // forward them to be processed
    void receive_send_selection_request();

};


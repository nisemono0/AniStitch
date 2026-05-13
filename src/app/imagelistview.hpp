#pragma once

#include "app/imagelistmodel.hpp"

#include <QWidget>
#include <QListView>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QKeyEvent>


class ImageListView : public QListView {
    Q_OBJECT;
public:
    explicit ImageListView(QWidget *parent = nullptr);
    ~ImageListView() override;

    // Return a std::vector of cv::Mat from all the items
    std::vector<cv::Mat> getItemsCVMats();

private:
    ImageListModel *image_list_model;

    QMenu *menu_context;
    QAction *action_delete_items;

    void setUpContextMenu();

    // Insert delete and clear items from the model
    void insert(const ImageItem &image_item);
    void deleteSelected();
    void clear();

    void sendImageListViewStatus();

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:
    // Send the current selected item's underlying QPixmap data
    void send_current_item_pixmap(const QPixmap &item_pixmap);
    // Send the ImageListView status text for image_list_status label
    void send_ImageListView_status(const QString &status);

public slots:
    // Insert image_item into ImageListView
    void receive_insert_item_request(const ImageItem &image_item);
    // Remove selected items from ImageListView
    void receive_delete_selected_items_request();
    // Remove all items from ImageListView
    void receive_clear_items_request();
    // Crop the slected items top_px/right_px/bottom_px/left_px number of pixels
    void receive_crop_selected_items_request(int top_px, int right_px, int bottom_px, int left_px);

private slots:
    void action_delete_items_triggered(bool checked = false);
};


#pragma once

#include "app/imagelistmodel.hpp"

#include <QWidget>
#include <QListView>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>


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
    void deleteSelectedItems();

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    // Send the current selected item's underlying QPixmap data
    void send_current_item_pixmap(const QPixmap &item_pixmap);

public slots:
    // Insert image_item into ImageListView
    void receive_insert_item_request(const ImageItem &image_item);
    // Remove selected items from ImageListView
    void receive_delete_items_request();
    // Remove all items from ImageListView
    void receive_clear_items_request();

private slots:
    void action_delete_items_triggered(bool checked = false);
};


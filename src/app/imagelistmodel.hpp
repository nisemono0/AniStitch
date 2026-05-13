#pragma once

#include "base/imageitem.hpp"

#include <QAbstractListModel>
#include <QModelIndex>
#include <QVariant>


class ImageListModel : public QAbstractListModel {
    Q_OBJECT;
public:
    // Custom roles
    enum ImageListRole {
        ITEM_PIXMAP = Qt::UserRole + 1,
        ITEM_CVMAT  = Qt::UserRole + 2,
    };

    explicit ImageListModel(QObject *parent = nullptr);
    ~ImageListModel();

    // Returns the number of items of parent
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    // Returns the item's data at role given an index
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Insert image_item into the model
    void insertItem(const ImageItem &image_item);
    // Delete the selected items from the model
    void deleteItems(const QModelIndexList &item_index_list);
    // Delete all the items from the model
    void clearItems();
    // True if the model has no items in it
    bool isEmpty();
    // Return the underlying QList<ImageItem> pointer that stores
    // the model's data as const so that it is read only
    const QList<ImageItem>* getModelData() const;

private:
    QList<ImageItem> *image_item_list;

};


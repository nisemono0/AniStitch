#include "app/imagelistmodel.hpp"

#include "utils/opencv.hpp"


ImageListModel::ImageListModel(QObject *parent) : QAbstractListModel(parent) {

}

ImageListModel::~ImageListModel() {

}

int ImageListModel::rowCount(const QModelIndex &parent) const {
    return this->image_item_list.size();
}

QVariant ImageListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= this->image_item_list.size() || index.row() < 0) {
        return QVariant();
    }

    const ImageItem &image_item = this->image_item_list.at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return image_item.display_name;
        case ImageListModel::ITEM_NAME:
            return image_item.name;
        case ImageListModel::ITEM_PIXMAP:
            return image_item.pixmap;
        case ImageListModel::ITEM_CVMAT:
            return QVariant::fromValue(image_item.cvmat);
        default:
            return QVariant();
            break;
    }
}

bool ImageListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid()) {
        return false;
    }
    int row = index.row();

    // Update stored cvmat and pixmap
    if (role == ImageListRole::ITEM_CVMAT) {
        if (!value.canConvert<cv::Mat>()) {
            return false;
        }

        ImageItem &image_item = this->image_item_list[row];
        image_item.display_name = this->image_item_list.value(row).name + QStringLiteral(": Cropped");
        image_item.name = this->image_item_list.value(row).name;
        image_item.cvmat = value.value<cv::Mat>();
        image_item.pixmap = Utils::Image::getPixmapFromMat(image_item.cvmat);

        emit dataChanged(
                    index, index,
                    { Qt::DisplayRole, ImageListRole::ITEM_NAME, ImageListRole::ITEM_PIXMAP, ImageListRole::ITEM_CVMAT }
                );

        return true;
    }

    return false;
}

Qt::ItemFlags ImageListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

void ImageListModel::insertItem(const ImageItem &image_item) {
    beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());

    this->image_item_list.append(image_item);

    endInsertRows();
}

void ImageListModel::deleteItems(const QModelIndexList &item_index_list) {
    if (item_index_list.isEmpty()) {
        return;
    }

    QList<int> rows_to_remove = QList<int>();

    for (auto &item_idx : item_index_list) {
        rows_to_remove.append(item_idx.row());
    }

    // Sort high to low the rows to be removed to fix shifting
    std::sort(rows_to_remove.begin(), rows_to_remove.end(), std::greater<int>());

    for (auto &row : rows_to_remove) {
        beginRemoveRows(QModelIndex(), row, row);
        this->image_item_list.removeAt(row);
        endRemoveRows();
    }
}

void ImageListModel::clearItems() {
    beginResetModel();

    this->image_item_list.clear();

    endResetModel();
}

bool ImageListModel::isEmpty() {
    if (this->rowCount() > 0) {
        return false;
    }
    return true;
}

const QList<ImageItem>& ImageListModel::getModelData() const {
    return this->image_item_list;
}


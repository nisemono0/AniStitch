#include "app/imagelistmodel.hpp"

#include "utils/opencv.hpp"


ImageListModel::ImageListModel(QObject *parent) : QAbstractListModel(parent) {
    this->image_item_list = new QList<ImageItem>();
}

ImageListModel::~ImageListModel() {
    delete this->image_item_list;
}

int ImageListModel::rowCount(const QModelIndex &parent) const {
    return this->image_item_list->size();
}

QVariant ImageListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= this->image_item_list->size() || index.row() < 0) {
        return QVariant();
    }

    const ImageItem &image_item = this->image_item_list->at(index.row());

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

        ImageItem new_image_item;
        new_image_item.display_name = this->image_item_list->value(row).name.append(QStringLiteral(": Cropped"));
        new_image_item.name = this->image_item_list->value(row).name;
        new_image_item.cvmat = value.value<cv::Mat>();
        new_image_item.pixmap = Utils::Image::getPixmapFromMat(new_image_item.cvmat);

        this->image_item_list->replace(row, new_image_item);

        emit dataChanged(index, index, { ImageListRole::ITEM_CVMAT, ImageListRole::ITEM_PIXMAP });

        return true;
    }

    return false;
}

void ImageListModel::insertItem(const ImageItem &image_item) {
    beginInsertRows(QModelIndex(), this->rowCount(), this->rowCount());

    this->image_item_list->append(image_item);

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
        this->image_item_list->removeAt(row);
        endRemoveRows();
    }
}

void ImageListModel::clearItems() {
    beginResetModel();

    this->image_item_list->clear();

    endResetModel();
}

bool ImageListModel::isEmpty() {
    if (this->rowCount() > 0) {
        return false;
    }
    return true;
}

const QList<ImageItem>* ImageListModel::getModelData() const {
    return this->image_item_list;
}


#include "app/imagelistview.hpp"

#include "utils/opencv.hpp"


ImageListView::ImageListView(QWidget *parent) : QListView(parent) {
    this->image_list_model = new ImageListModel(this);
    this->setModel(this->image_list_model);

    this->setUpContextMenu();
}

ImageListView::~ImageListView() {
    delete this->action_delete_items;
    delete this->menu_context;

    delete this->image_list_model;
}

std::vector<cv::Mat> ImageListView::getItemsCVMats() {
    const QList<ImageItem> *model_data = this->image_list_model->getModelData();

    std::vector<cv::Mat> cv_mats = std::vector<cv::Mat>();

    for (auto &data : *model_data) {
        cv_mats.push_back(data.cvmat);
    }

    return cv_mats;
}

void ImageListView::setUpContextMenu() {
    // Create context menu and add item to it
    this->menu_context = new QMenu(QStringLiteral("Images context menu"), this);
    this->action_delete_items = new QAction(QStringLiteral("Delete item(s)"), this->menu_context);

    this->menu_context->addAction(this->action_delete_items);

    // Connect to the corresponding context menu actions
    connect(this->action_delete_items, &QAction::triggered, this, &ImageListView::action_delete_items_triggered);
}

void ImageListView::receive_insert_item_request(const ImageItem &image_item) {
    this->insert(image_item);
}

void ImageListView::receive_delete_selected_items_request() {
    this->deleteSelected();
}

void ImageListView::receive_clear_items_request() {
    this->clear();
}

void ImageListView::receive_crop_selected_items_request(int top_px, int right_px, int bottom_px, int left_px) {
    QModelIndexList selected_idx = this->selectedIndexes();

    QModelIndex current_selected_idx = this->selectionModel()->currentIndex();

    for (auto &idx : selected_idx) {
        if (idx.data(ImageListModel::ITEM_CVMAT).canConvert<cv::Mat>()) {
            QVariant cropped_cvmat_variant = QVariant::fromValue(
                        Utils::Image::cropMat(
                                idx.data(ImageListModel::ITEM_CVMAT).value<cv::Mat>(),
                                top_px, right_px, bottom_px, left_px
                            )
                    );
            this->image_list_model->setData(idx, cropped_cvmat_variant, ImageListModel::ITEM_CVMAT);

            // Manually trigger the currentChanged slot with the last selected index
            // this way if the selecte item's image gets updated it will be displayed
            this->currentChanged(current_selected_idx, current_selected_idx);
        }
    }
}

void ImageListView::action_delete_items_triggered(bool checked) {
    this->deleteSelected();
}

void ImageListView::insert(const ImageItem &image_item) {
    this->image_list_model->insertItem(image_item);
    this->sendImageListViewStatus();
}

void ImageListView::deleteSelected() {
    this->image_list_model->deleteItems(
            this->selectedIndexes()
            );
    this->sendImageListViewStatus();
}

void ImageListView::clear() {
    this->image_list_model->clearItems();
    this->sendImageListViewStatus();
}

void ImageListView::sendImageListViewStatus() {
    int image_list_size = this->image_list_model->rowCount();
    if (image_list_size > 0) {
        emit send_ImageListView_status(
                QStringLiteral("Loaded images: %1").arg(
                    QString::number(image_list_size)
                    )
                );
    } else {
        emit send_ImageListView_status(QStringLiteral("No images loaded"));
    }
}

void ImageListView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    // Send an empty QPixmap if the current selection is not valid
    // This can happen when you manually delete all images instead
    // of clearing all
    if (!current.isValid()) {
        emit send_current_item_pixmap(QPixmap());
        return;
    }

    // Check if QPixmap can be retrieved from the item's data
    if (!current.data(ImageListModel::ITEM_PIXMAP).canConvert<QPixmap>()) {
        return;
    }

    // Make sure the selected item is visible in the list
    this->scrollTo(current, QListView::EnsureVisible);

    QPixmap item_pixmap = current.data(ImageListModel::ITEM_PIXMAP).value<QPixmap>();
    emit send_current_item_pixmap(item_pixmap);
}

void ImageListView::contextMenuEvent(QContextMenuEvent *event) {
    QListView::contextMenuEvent(event);
    if (!this->image_list_model->isEmpty()) {
        this->menu_context->popup(event->globalPos());
    }
}

void ImageListView::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        // Remove selected items on DEL key press
        case Qt::Key_Delete:
        {
            this->deleteSelected();
            break;
        }
    }
    QListView::keyPressEvent(event);
}


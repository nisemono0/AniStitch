#include "app/imagelistview.hpp"


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
    this->image_list_model->insertItem(image_item);
}

void ImageListView::receive_delete_items_request() {
    this->deleteSelectedItems();
}

void ImageListView::receive_clear_items_request() {
    this->image_list_model->clearItems();
}

void ImageListView::action_delete_items_triggered(bool checked) {
    this->deleteSelectedItems();
}

void ImageListView::deleteSelectedItems() {
    this->image_list_model->deleteItems(
            this->selectedIndexes()
            );
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
    if (!current.data(ImageListModel::ItemPixmap).canConvert<QPixmap>()) {
        return;
    }

    QPixmap item_pixmap = current.data(ImageListModel::ItemPixmap).value<QPixmap>();
    emit send_current_item_pixmap(item_pixmap);
}

void ImageListView::contextMenuEvent(QContextMenuEvent *event) {
    QListView::contextMenuEvent(event);
    if (!this->image_list_model->isEmpty()) {
        this->menu_context->popup(event->globalPos());
    }
}


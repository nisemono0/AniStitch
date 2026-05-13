#include "base/imageloader.hpp"

#include "utils/log.hpp"
#include "utils/file.hpp"
#include "utils/opencv.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

#include <QThread>


ImageLoader::ImageLoader(QObject *parent) : QObject(parent) {
    this->image_counter = 0;
}

ImageLoader::~ImageLoader() {

}

QString ImageLoader::getNameAndUpdateCounter() {
    this->image_counter++;
    return QStringLiteral("Image %1").arg(QString::number(this->image_counter));
}

std::optional<ImageItem> ImageLoader::getImageItemFromImage(const QString &image_path) {
    cv::Mat image_mat = cv::imread(image_path.toStdString(), cv::IMREAD_UNCHANGED);

    if (image_mat.empty()) {
        return std::nullopt;
    }

    ImageItem image_item = ImageItem();
    image_item.display_name = this->getNameAndUpdateCounter();
    image_item.name = image_item.display_name;
    image_item.pixmap = Utils::Image::getPixmapFromMat(image_mat);
    image_item.cvmat = image_mat.clone();

    return image_item;
}

QList<std::optional<ImageItem>> ImageLoader::getImageItemFromVideo(const QString &video_path) {
    // Open video_path as video
    cv::VideoCapture video_cap = cv::VideoCapture(video_path.toStdString(), cv::CAP_ANY);
    // If the video couldn't be opened, return and empty QList
    if (!video_cap.isOpened()) {
        return QList<std::optional<ImageItem>>();
    }

    QList<std::optional<ImageItem>> image_item_list = QList<std::optional<ImageItem>>();
    cv::Mat video_frame = cv::Mat();

    int frame_count = video_cap.get(cv::CAP_PROP_FRAME_COUNT);
    for (int frame_idx = 0; frame_idx <= frame_count; frame_idx++) {
        bool frame_grabbed = video_cap.read(video_frame);
        if (frame_grabbed) {
            ImageItem image_item = ImageItem();

            image_item.display_name = this->getNameAndUpdateCounter();
            image_item.name = image_item.display_name;
            image_item.pixmap = Utils::Image::getPixmapFromMat(video_frame);
            image_item.cvmat = video_frame.clone();

            image_item_list.append(image_item);
        } else {
            image_item_list.append(std::nullopt);
        }
    }

    // Close video capture
    video_cap.release();

    return image_item_list;
}

void ImageLoader::receive_ImageLoader_start_request(const QStringList &file_paths) {
    // Request show the progress bar
    emit send_ImageLoader_show_progress_bar();

    // Checker for warnings and such
    bool has_warnings = false;
    bool has_errors = false;

    int total_files = file_paths.size();

    for (int file_idx = 0; file_idx < total_files; file_idx++) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            emit send_ImageLoader_status(ImageLoaderStatus::INTERRUPTED);
            return;
        }

        // Reset the per file progress bar
        emit send_ImageLoader_reset_current_progress();

        QString current_file_path = file_paths[file_idx];
        // Load image
        if (Utils::File::isImage(current_file_path)) {
            Log::info(QStringLiteral("Loading image: %1").arg(current_file_path));

            std::optional<ImageItem> image_item = this->getImageItemFromImage(current_file_path);
            // If item has no image loaded, send error and stop loading images
            if (image_item) {
                Log::info(QStringLiteral("  -> Image (%1) loaded: %2").arg(image_item.value().display_name, current_file_path));
                emit send_ImageLoader_data(image_item.value());
            } else {
                Log::error(QStringLiteral("Error loading image: %1").arg(current_file_path));
                has_errors = true;
            }
            // Send 100 since it process a single file
            emit send_ImageLoader_current_progress(100);
        // Load video frames
        } else if (Utils::File::isVideo(current_file_path)) {
            Log::info(QStringLiteral("Loading video: %1").arg(current_file_path));

            QList<std::optional<ImageItem>> image_item_list = this->getImageItemFromVideo(current_file_path);
            if (image_item_list.isEmpty()) {
                Log::error(QStringLiteral("Error loading video: %1").arg(current_file_path));
                has_errors = true;
            }

            int total_items = image_item_list.size();
            for (int item_idx = 0; item_idx < total_items; item_idx++) {
                if (QThread::currentThread()->isInterruptionRequested()) {
                    // Update counter
                    this->image_counter = this->image_counter - (total_items - item_idx);
                    emit send_ImageLoader_status(ImageLoaderStatus::INTERRUPTED);
                    return;
                }
                // If item has no image loaded, send error and stop loading images
                if (image_item_list[item_idx]) {
                    Log::info(QStringLiteral("  -> Frame (%1) loaded: %2").arg(image_item_list[item_idx].value().display_name, current_file_path));
                    emit send_ImageLoader_data(image_item_list[item_idx].value());
                } else {
                    Log::warning(QStringLiteral("  -> Frame not loaded: %1").arg(current_file_path));
                    has_warnings = true;
                }
                // Send current video frame load progress
                emit send_ImageLoader_current_progress((item_idx + 1) * 100 / total_items);
            }
        // File is not image of video
        } else {
            Log::warning(QStringLiteral("Not an image/video: %1").arg(current_file_path));
            has_warnings = true;
        }
        // Send total files processed progress
        emit send_ImageLoader_total_progress((file_idx + 1) * 100 / total_files);
    }

    if (has_errors) {
        emit send_ImageLoader_status(ImageLoaderStatus::ERROR);
        return;
    }

    if (has_warnings) {
        emit send_ImageLoader_status(ImageLoaderStatus::WARNING);
        return;
    }

    emit send_ImageLoader_status(ImageLoaderStatus::OK);
}

void ImageLoader::receive_ImageLoader_reset_counter() {
    this->image_counter = 0;
}


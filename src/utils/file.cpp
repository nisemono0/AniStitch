#include "utils/file.hpp"
#include "utils/str.hpp"

#include "base/settings.hpp"

#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMimeDatabase>
#include <QMimeType>


bool Utils::File::fileExists(const QString &file_path) {
    if (QFile::exists(file_path)) {
        return true;
    }
    return false;
}

bool Utils::File::isVideo(const QString &file_path) {
    QMimeDatabase mime_db = QMimeDatabase();
    QMimeType mime_type = mime_db.mimeTypeForFile(file_path);

    if (!mime_type.isValid()) {
        return false;
    }

    if (mime_type.name().startsWith(QStringLiteral("video/"))) {
        return true;
    }
    return false;
}

bool Utils::File::isImage(const QString &file_path) {
    QMimeDatabase mime_db = QMimeDatabase();
    QMimeType mime_type = mime_db.mimeTypeForFile(file_path);

    if (!mime_type.isValid()) {
        return false;
    }

    if (mime_type.name().startsWith(QStringLiteral("image/"))) {
        return true;
    }
    return false;
}

bool Utils::File::containsExtension(const QString &file_path, const QStringList &ext_list) {
    QFileInfo file_info = QFileInfo(file_path);
    return ext_list.contains(file_info.suffix(), Qt::CaseInsensitive);
}

QString Utils::File::absolutePath(const QString &file_path) {
    QFileInfo file_info = QFileInfo(file_path);
    return file_info.absolutePath();
}

QString Utils::File::sanitizeFilePath(const QString &file_path, const QString &extension) {
    QFileInfo file_info = QFileInfo(file_path);
    QString absolute_path = file_info.absolutePath();

    if (Utils::String::isNullOrEmpty(extension)) {
        QString sanitized_filename = file_info.completeBaseName().replace(".", "_") + "." + file_info.suffix();
        return QDir(absolute_path).filePath(sanitized_filename);
    }

    QString sanitized_filename = file_info.fileName().replace(".", "_") + "." + extension;
    return QDir(absolute_path).filePath(sanitized_filename);
}

QStringList Utils::FileDialog::openFile(QWidget *parent) {
    QFileDialog open_dialog = QFileDialog(parent, QStringLiteral("Select file(s)"));

    open_dialog.setDirectory(Settings::last_open_file_path);
    open_dialog.setAcceptMode(QFileDialog::AcceptOpen);
    open_dialog.setFileMode(QFileDialog::ExistingFiles);
    open_dialog.setNameFilters({
            QStringLiteral("Video file(s) (*.webm *.mp4 *.mkv)"),
            QStringLiteral("Image file(s) (*.png *.jpg *.jpeg *.webp)")
            });

    if (open_dialog.exec()) {
        QStringList selected_files = open_dialog.selectedFiles();
        // Set last open file dialog path to the first selected file
        Settings::last_open_file_path = Utils::File::absolutePath(selected_files.first());
        return selected_files;
    }
    return QStringList();
}

QString Utils::FileDialog::saveImage(QWidget *parent) {
    QFileDialog save_dialog = QFileDialog(parent, QStringLiteral("Save stitch"));

    save_dialog.setDirectory(Settings::last_save_stitch_path);
    save_dialog.setAcceptMode(QFileDialog::AcceptSave);
    save_dialog.setFileMode(QFileDialog::AnyFile);
    save_dialog.setNameFilter(
            QStringLiteral("Image file (*.png *.jpg *.jpeg *.webp)")
            );
    save_dialog.setDefaultSuffix(QStringLiteral(".png"));

    if (save_dialog.exec()) {
        QString save_path = save_dialog.selectedFiles().first();
        // Set last save dialog path to the selected file
        Settings::last_save_stitch_path = Utils::File::absolutePath(save_path);
        // Return sanitized save_path
        if (Utils::File::containsExtension(save_path, {"png", "jpg", "jpeg", "webp"})) {
            return Utils::File::sanitizeFilePath(save_path);
        }
        return Utils::File::sanitizeFilePath(save_path, "png");
    }
    return QString();
}

QString Utils::FileDialog::saveLog(QWidget *parent) {
    QFileDialog save_dialog = QFileDialog(parent, QStringLiteral("Save log"));

    save_dialog.setDirectory(Settings::last_save_log_path);
    save_dialog.setAcceptMode(QFileDialog::AcceptSave);
    save_dialog.setFileMode(QFileDialog::AnyFile);
    save_dialog.setNameFilter(
            QStringLiteral("Log file (*.log *.txt)")
            );
    save_dialog.setDefaultSuffix(QStringLiteral(".log"));

    if (save_dialog.exec()) {
        QString save_path = save_dialog.selectedFiles().first();
        // Set last save dialog path to the selected file
        Settings::last_save_log_path = Utils::File::absolutePath(save_path);
        // Return sanitized save_path
        if (Utils::File::containsExtension(save_path, {"log"})) {
            return Utils::File::sanitizeFilePath(save_path);
        }
        return Utils::File::sanitizeFilePath(save_path, "log");
    }
    return QString();
}


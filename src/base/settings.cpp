#include "base/settings.hpp"

#include <QDir>


QSettings Settings::settings = QSettings(Utils::App::OrgName, Utils::App::AppName);

QString Settings::last_open_file_path = QDir::homePath();
QString Settings::last_save_stitch_path = QDir::homePath();
QString Settings::last_save_log_path = QDir::homePath();

void Settings::saveSettings() {
    Settings::settings.setValue(QStringLiteral("last_open_file_path"), Settings::last_open_file_path);
    Settings::settings.setValue(QStringLiteral("last_save_stitch_path"), Settings::last_save_stitch_path);
    Settings::settings.setValue(QStringLiteral("last_save_log_path"), Settings::last_save_log_path);
}

void Settings::loadSettings() {
    Settings::last_open_file_path = Settings::settings.value(QStringLiteral("last_open_file_path"), QDir::homePath()).toString();
    Settings::last_save_stitch_path = Settings::settings.value(QStringLiteral("last_save_stitch_path"), QDir::homePath()).toString();
    Settings::last_save_log_path = Settings::settings.value(QStringLiteral("last_save_log_path"), QDir::homePath()).toString();
}


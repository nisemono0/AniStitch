#pragma once

#include "utils/defs.hpp"

#include <QString>
#include <QSettings>


class Settings {
public:
    // Last open file path
    static QString last_open_file_path;
    // Last save stitch path
    static QString last_save_stitch_path;
    // Last save log path
    static QString last_save_log_path;

    // Save settings
    static void saveSettings();
    // Load settings
    static void loadSettings();

private:
    static QSettings settings;
};


#include "gui/mainwindow.hpp"

#include "base/settings.hpp"

#include "utils/defs.hpp"
#include "utils/opencv.hpp"
#include "utils/metatypes.hpp"


int main (int argc, char *argv[]) {
    // Enable OpenCL if avaialble
    Utils::OpenCV::enableOpenCL();

    QApplication app(argc, argv);

    // Register metatypes
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<std::vector<cv::Mat>>("std::vector<cv::Mat>");
    qRegisterMetaType<StitcherSettings>("StitcherSettings");

    app.setApplicationName(App::AppName);
    app.setOrganizationName(App::OrgName);

    app.setCursorFlashTime(App::CursorBlinkInterval);

    app.setStyle(App::Style);

    // Load saved settings
    Settings::loadSettings();

    MainWindow main_window;

    // Set window as dialog so my wm open this as
    // floating w/o configuring a rule for it
    // main_window.setWindowFlag(Qt::Dialog);
    main_window.show();

    int app_return_code = app.exec();

    if (app_return_code == 0) {
        Settings::saveSettings();
    }

    return app_return_code;
}

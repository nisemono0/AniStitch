#include "gui/mainwindow.hpp"

#include "base/settings.hpp"

#include "utils/defs.hpp"
#include "utils/opencv.hpp"
#include "utils/metatypes.hpp"

#include <QCommandLineParser>


int main (int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Register metatypes
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<std::vector<cv::Mat>>("std::vector<cv::Mat>");
    qRegisterMetaType<StitcherSettings>("StitcherSettings");

    // Set some app settings
    app.setApplicationName(Utils::App::AppName);
    app.setOrganizationName(Utils::App::OrgName);

    app.setCursorFlashTime(Utils::App::CursorBlinkInterval);

    app.setStyle(Utils::App::Style);

    // Arguments parser
    QCommandLineParser arg_parser;
    arg_parser.setApplicationDescription(Utils::App::AppDesc);
    arg_parser.addHelpOption();

    QCommandLineOption dialog_window_option = {{"window", "w"}, "Start the application as a dialog window."};
    arg_parser.addOption(dialog_window_option);

    arg_parser.addPositionalArgument("file_paths", "List of files to load into the program.", "[file_paths...]");

    arg_parser.process(app);

    QStringList file_list = arg_parser.positionalArguments();

    // Disable OpenCL if avaialble
    Utils::OpenCV::disableOpenCL();

    // Load saved settings
    Settings::loadSettings();

    MainWindow main_window = MainWindow(file_list);

    // Set window hint to dialog if started with --window/-w
    // This makes the window floating on tiling WM without
    // having to setup exception rules for it
    if (arg_parser.isSet(dialog_window_option)) {
        main_window.setWindowFlag(Qt::Dialog, true);
    }

    main_window.show();

    int app_return_code = app.exec();
    if (app_return_code == 0) {
        Settings::saveSettings();
    }

    return app_return_code;
}

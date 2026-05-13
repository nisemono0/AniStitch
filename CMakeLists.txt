cmake_minimum_required(VERSION 3.16)
project(AniStitch LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(AniStitch
    src/app/displayimagescene.cpp
    src/app/displayimagescene.hpp
    src/app/displayimageview.cpp
    src/app/displayimageview.hpp
    src/app/dualprogressdialog.cpp
    src/app/dualprogressdialog.hpp
    src/app/imagegraphicsscene.cpp
    src/app/imagegraphicsscene.hpp
    src/app/imagegraphicsview.cpp
    src/app/imagegraphicsview.hpp
    src/app/imagelistmodel.cpp
    src/app/imagelistmodel.hpp
    src/app/imagelistview.cpp
    src/app/imagelistview.hpp
    src/app/progressdialog.cpp
    src/app/progressdialog.hpp
    src/base/imageitem.cpp
    src/base/imageitem.hpp
    src/base/imageloader.cpp
    src/base/imageloader.hpp
    src/base/imagestitcher.cpp
    src/base/imagestitcher.hpp
    src/base/settings.cpp
    src/base/settings.hpp
    src/gui/cropdialog.cpp
    src/gui/cropdialog.hpp
    src/gui/displayimagedialog.cpp
    src/gui/displayimagedialog.hpp
    src/gui/logdialog.cpp
    src/gui/logdialog.hpp
    src/gui/mainwindow.cpp
    src/gui/mainwindow.hpp
    src/gui/stitchersettingsdialog.cpp
    src/gui/stitchersettingsdialog.hpp
    src/main.cpp
    src/utils/defs.hpp
    src/utils/file.cpp
    src/utils/file.hpp
    src/utils/log.cpp
    src/utils/log.hpp
    src/utils/metatypes.hpp
    src/utils/opencv.cpp
    src/utils/opencv.hpp
    src/utils/str.cpp
    src/utils/str.hpp
)

target_sources(AniStitch
    PRIVATE
        src/app/ui/dualprogressdialog.ui
        src/gui/ui/cropdialog.ui
        src/gui/ui/displayimagedialog.ui
        src/gui/ui/logdialog.ui
        src/gui/ui/mainwindow.ui
        src/gui/ui/stitchersettingsdialog.ui
)

target_link_libraries(AniStitch
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

# !!! cmakeconf.sh !!!
find_package(Qt6 REQUIRED COMPONENTS Concurrent)

find_package(OpenCV REQUIRED COMPONENTS core)
find_package(OpenCV REQUIRED COMPONENTS imgcodecs)
find_package(OpenCV REQUIRED COMPONENTS imgproc)
find_package(OpenCV REQUIRED COMPONENTS videoio)
find_package(OpenCV REQUIRED COMPONENTS stitching)

target_include_directories(AniStitch PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(AniStitch PRIVATE Qt::Concurrent)
target_link_libraries(AniStitch PRIVATE ${OpenCV_LIBS})

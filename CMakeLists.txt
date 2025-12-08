cmake_minimum_required(VERSION 3.16)
project(ReLib LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(ReLib
    src/app/imagescene.cpp
    src/app/imagescene.hpp
    src/app/imageview.cpp
    src/app/imageview.hpp
    src/app/librarymodel.cpp
    src/app/librarymodel.hpp
    src/app/librarymodelproxy.cpp
    src/app/librarymodelproxy.hpp
    src/app/libraryview.cpp
    src/app/libraryview.hpp
    src/app/libraryviewdelegate.cpp
    src/app/libraryviewdelegate.hpp
    src/app/searchcompleter.cpp
    src/app/searchcompleter.hpp
    src/app/searchplaintextedit.cpp
    src/app/searchplaintextedit.hpp
    src/base/clipboard.cpp
    src/base/clipboard.hpp
    src/base/dbworker.cpp
    src/base/dbworker.hpp
    src/base/imageworker.cpp
    src/base/imageworker.hpp
    src/base/log.cpp
    src/base/log.hpp
    src/base/manga.cpp
    src/base/manga.hpp
    src/base/mangatextedit.cpp
    src/base/mangatextedit.hpp
    src/base/settings.cpp
    src/base/settings.hpp
    src/base/shortcuts.cpp
    src/base/shortcuts.hpp
    src/base/zipworker.cpp
    src/base/zipworker.hpp
    src/gui/logdialog.cpp
    src/gui/logdialog.hpp
    src/gui/mainwindow.cpp
    src/gui/mainwindow.hpp
    src/gui/mangainfodialog.cpp
    src/gui/mangainfodialog.hpp
    src/main.cpp
    src/utils/defs.hpp
    src/utils/fs.cpp
    src/utils/fs.hpp
    src/utils/json.cpp
    src/utils/json.hpp
    src/utils/str.cpp
    src/utils/str.hpp
    src/utils/zip.cpp
    src/utils/zip.hpp
)

target_sources(ReLib
    PRIVATE
        src/gui/ui/logdialog.ui
        src/gui/ui/mainwindow.ui
        src/gui/ui/mangainfodialog.ui
)

qt_add_resources(ReLib_resources src/gui/theme/mouse-next-prev.qrc)
target_sources(ReLib
    PRIVATE
        ${ReLib_resources}
)

target_link_libraries(ReLib
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

# !!! cmakeconf.sh !!!
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)
find_package(QuaZip-Qt6)
target_include_directories(ReLib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(ReLib PRIVATE Qt::Sql Qt::Concurrent QuaZip::QuaZip)

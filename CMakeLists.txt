cmake_minimum_required(VERSION 3.16)
project(ReLib LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(ReLib
    src/app/imageview.cpp
    src/app/imageview.hpp
    src/app/librarymodel.cpp
    src/app/librarymodel.hpp
    src/app/librarymodelproxy.cpp
    src/app/librarymodelproxy.hpp
    src/app/libraryview.cpp
    src/app/libraryview.hpp
    src/base/dbmanager.cpp
    src/base/dbmanager.hpp
    src/base/log.cpp
    src/base/log.hpp
    src/base/manga.cpp
    src/base/manga.hpp
    src/base/zipmanager.cpp
    src/base/zipmanager.hpp
    src/gui/logdialog.cpp
    src/gui/logdialog.hpp
    src/gui/mainwindow.cpp
    src/gui/mainwindow.hpp
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

#!/bin/bash

PROJ_NAME=$(basename "$PWD")

rm -rf ./build
rm -f ./CMakeLists.txt

/usr/lib/qt6/bin/qt-cmake-create

echo
echo "[!!!] cmakeconf.sh: CMakeLists.txt config"

sed -i -e '4i set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n' ./CMakeLists.txt
echo "[!!!] Added set(CMAKE_EXPORT_COMPILE_COMMANDS ON) on line 4 of CMakeLists.txt (needed for .clangd)"
echo "" >> ./CMakeLists.txt

echo "# !!! cmakeconf.sh !!!" >> ./CMakeLists.txt
echo "find_package(Qt6 REQUIRED COMPONENTS Sql)" >> ./CMakeLists.txt
echo "find_package(Qt6 REQUIRED COMPONENTS Concurrent)" >> ./CMakeLists.txt
echo "find_package(QuaZip-Qt6)" >> ./CMakeLists.txt

echo "target_include_directories($PROJ_NAME PUBLIC \${CMAKE_SOURCE_DIR}/src)" >> ./CMakeLists.txt

echo "target_link_libraries($PROJ_NAME PRIVATE Qt::Sql Qt::Concurrent QuaZip::QuaZip)" >> ./CMakeLists.txt

echo "[!!!] (Optional) Manually merge cmakeconf.sh added options in CMakeLists.txt"

if [ "$1" == "--compile" ]; then
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build -j $(nrproc --all)
fi

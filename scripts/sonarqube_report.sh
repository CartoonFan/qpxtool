#!/bin/zsh

cd "${MESON_SOURCE_ROOT}"
meson "${MESON_SOURCE_ROOT}"/build
meson "${MESON_SOURCE_ROOT}"/clang-build
meson compile --clean -C "${MESON_BUILD_ROOT}"
meson compile --clean -C "${MESON_SOURCE_ROOT}"/clang-build
chmod -R 777 build
chmod -R 777 clang-build

cd "${MESON_BUILD_ROOT}"
ninja clang-tidy > "${MESON_BUILD_ROOT}/clang-tidy-report"
cd "${MESON_SOURCE_ROOT}"
cppcheck -v --enable=all --xml -f --report-progress -I /usr/include/c++/10.2.0/ -I/usr/include/c++/10.2.0/tr1/ -I /usr/include/ -I /usr/include/c++/10.2.0/x86_64-pc-linux-gnu/ -I /usr/include/linux/ -I /usr/include/libpng16/ -I /usr/lib/qt/mkspecs/linux-g++/ -I /usr/include/qt/QtCore/ -I /usr/include/qt/QtNetwork/ -I /usr/include/qt/QtGui/ -I /usr/include/qt/QtSql/ -I /usr/include/qt/ -I "${MESON_SOURCE_ROOT}"/include/ -I /usr/lib/zig/libc/include/x86_64-linux-gnux32/ -D__cplusplus="201702L" . 2> "${MESON_BUILD_ROOT}"/cppcheck-report.xml
meson compile -C "${MESON_BUILD_ROOT}" > "${MESON_BUILD_ROOT}"/gcc-build.log

# Currently have to modify scanbuild.py with correct args
# to get it to work properly with sonarqube
ninja -C "${MESON_SOURCE_ROOT}"/clang-build scan-build 
meson compile --clean -C "${MESON_BUILD_ROOT}"
meson compile --clean -C "${MESON_SOURCE_ROOT}"/clang-build

rats -w 3 --xml src/* > "${MESON_BUILD_ROOT}"/rats-report.xml


sed -i -- 's/\.\.\///g' "${MESON_BUILD_ROOT}/clang-tidy-report"
sed -i -- 's/\/home\/jeremiah\/workshop\/qpxtool\///g' "${MESON_BUILD_ROOT}/clang-tidy-report"
sed -i -- 's/build\///g' "${MESON_BUILD_ROOT}/clang-tidy-report"
sed -i -- 's/\.\.\///g' "${MESON_BUILD_ROOT}/gcc-build.log"
sed -i -- 's/\.\.\/\.\.\/\.\.\///g' "${MESON_SOURCE_ROOT}"/clang-build/meson-logs/scanbuild/**/*.plist(D.)

NODE_PATH=/usr/lib/node_modules sonar-scanner -Dsonar.projectBaseDir="${MESON_SOURCE_ROOT}"

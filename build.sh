#!/bin/bash
echo "  _________              __  .__              .__    __________      .__.__       .___"
echo " /   _____/ ____   _____/  |_|__| ____   ____ |  |   \______   \__ __|__|  |    __| _/"
echo " \_____  \_/ __ \ /    \   __\  |/    \_/ __ \|  |    |    |  _/  |  \  |  |   / __ | "
echo " /        \  ___/|   |  \  | |  |   |  \  ___/|  |__  |    |   \  |  /  |  |__/ /_/ | "
echo "/_______  /\___  >___|  /__| |__|___|  /\___  >____/  |______  /____/|__|____/\____ | "
echo "        \/     \/     \/             \/     \/               \/                    \/ "

BUILD_DIR=build
TOOLCHAIN_FILE=mingw-toolchain.cmake

rm -rf $BUILD_DIR

# ----------------------
# ---- Linux Builds ----
# ----------------------

mkdir -p $BUILD_DIR/linux_release
cd $BUILD_DIR/linux_release

cmake -DCMAKE_BUILD_TYPE=Release ../..
cmake --build . --config Release

cd ../..

mkdir -p $BUILD_DIR/linux_release_avx
cd $BUILD_DIR/linux_release_avx

cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_AVX=ON ../..
cmake --build . --config Release

cd ../..

mkdir -p $BUILD_DIR/linux_release_sse
cd $BUILD_DIR/linux_release_sse

cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_SSE=ON ../..
cmake --build . --config Release

cd ../..

# ----------------------
# --- Windows Builds ---
# ----------------------

mkdir -p $BUILD_DIR/windows_release
cd $BUILD_DIR/windows_release

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../../$TOOLCHAIN_FILE ../..
cmake --build . --config Release

cd ../..

mkdir -p $BUILD_DIR/windows_release_avx
cd $BUILD_DIR/windows_release_avx

cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_AVX=ON -DCMAKE_TOOLCHAIN_FILE=../../$TOOLCHAIN_FILE ../..
cmake --build . --config Release

cd ../..

mkdir -p $BUILD_DIR/windows_release_sse
cd $BUILD_DIR/windows_release_sse

cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_SSE=ON -DCMAKE_TOOLCHAIN_FILE=../../$TOOLCHAIN_FILE ../..
cmake --build . --config Release

echo "Done"

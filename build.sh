#!/usr/bin/env bash
# Build MultiApp (Conan + CMake). Run from anywhere.
# Usage: ./build.sh [release|debug]  (default: release)
set -euo pipefail
cd "$(dirname "$0")"

BUILD_TYPE="${1:-debug}"
BUILD_TYPE_LOWER="$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')"

case "$BUILD_TYPE_LOWER" in
    release|r)
        CONAN_BUILD_TYPE="Release"
        CMAKE_PRESET="conan-release"
        EXE_PATH="build/build/Release/multiapp"
        ;;
    debug|d)
        CONAN_BUILD_TYPE="Debug"
        CMAKE_PRESET="conan-debug"
        EXE_PATH="build/build/Debug/multiapp"
        ;;
    *)
        echo "Usage: $0 [release|debug]"
        echo "  release (default) - optimized build (-O3)"
        echo "  debug             - debug symbols (-g -O0)"
        exit 1
        ;;
esac

echo "Building $CONAN_BUILD_TYPE..."

# 1. Install dependencies for the build type
conan install . --output-folder=build --build=missing -s build_type="$CONAN_BUILD_TYPE"

# 2. Configure (uses the conan-<type> preset Conan just generated)
cmake --preset "$CMAKE_PRESET"

# 3. Build
cmake --build --preset "$CMAKE_PRESET"

echo "Built: $EXE_PATH"
echo "Run from the repo root (it loads resources/gtk/main_app.ui):"
echo "  ./$EXE_PATH"


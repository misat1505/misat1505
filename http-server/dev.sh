#!/usr/bin/env bash
set -e

BUILD_DIR="build"

# Ensure build directory exists and run cmake
function ensure_build() {
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    if [ ! -f "Makefile" ]; then
        cmake ..
    fi
    make
    cd ..
}

function build() {
    ensure_build
}

function run() {
    ensure_build
    ./build/HttpServer
}

function test_all() {
    ensure_build
    ./build/HttpServer_tests
}

function clean() {
    rm -rf "$BUILD_DIR"
}

function format() {
    find . -type f \
        -regex '.*\.\(cpp\|hpp\|h\|cxx\)' \
        -not -path "./build/*" \
        -exec clang-format -i {} \;
}

case "$1" in
    build)
        build
        ;;
    run)
        run
        ;;
    test)
        test_all
        ;;
    clean)
        clean
        ;;
    format)
        format
        ;;
    *)
        echo "Usage: $0 {build|run|test|clean|format}"
        exit 1
        ;;
esac

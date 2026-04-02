#!/bin/sh
set -e

if command -v gcc >/dev/null 2>&1; then
    CC="gcc"
elif command -v clang >/dev/null 2>&1; then
    CC="clang"
else
    echo "Error: gcc or clang not found."
    exit 1
fi

if ! command -v curl >/dev/null 2>&1 || ! command -v tar >/dev/null 2>&1; then
    echo "Error: curl and tar are required."
    exit 1
fi

OS="$(uname -s)"
EXT=""
SUDO="sudo"
INSTALL_DIR="/usr/local/bin"

case "$OS" in
    Linux*)
        if [ -n "$PREFIX" ] && [ -n "$TERMUX_VERSION" ]; then
            SUDO=""
            INSTALL_DIR="$PREFIX/bin"
        fi
        ;;
    Darwin*)
        ;;
    MINGW*|CYGWIN*|MSYS*)
        SUDO=""
        EXT=".exe"
        if [ ! -d "$INSTALL_DIR" ]; then
            INSTALL_DIR="$HOME/bin"
        fi
        ;;
    *)
        ;;
esac

# Create a temporary directory and ensure it gets deleted on exit
TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

echo "Downloading icrawl source code..."
cd "$TMP_DIR"
curl -fsSL https://github.com/sapirrior/icrawl/archive/refs/heads/main.tar.gz | tar -xz
cd icrawl-main

echo "Building icrawl using $CC..."
$CC -Wall -Wextra -std=c11 -O2 -Iinclude source/main.c source/engine.c -o "icrawl$EXT" -lcurl

chmod +x "icrawl$EXT"

echo "Installing to $INSTALL_DIR..."
if [ -n "$SUDO" ]; then
    $SUDO mkdir -p "$INSTALL_DIR"
    $SUDO mv "icrawl$EXT" "$INSTALL_DIR/icrawl$EXT"
else
    mkdir -p "$INSTALL_DIR"
    mv "icrawl$EXT" "$INSTALL_DIR/icrawl$EXT"
fi

echo "Success. icrawl is installed."

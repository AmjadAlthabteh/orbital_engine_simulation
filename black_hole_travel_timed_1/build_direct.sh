#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

IMGUI_DIR="${IMGUI_DIR:-}"
IMGUI_SFML_DIR="${IMGUI_SFML_DIR:-}"

if [[ -z "$IMGUI_DIR" ]]; then
    if [[ -f /mnt/c/Dev/imgui/imgui.cpp ]]; then
        IMGUI_DIR=/mnt/c/Dev/imgui
    elif [[ -f C:/Dev/imgui/imgui.cpp ]]; then
        IMGUI_DIR=C:/Dev/imgui
    fi
fi

if [[ -z "$IMGUI_SFML_DIR" ]]; then
    if [[ -f /mnt/c/Dev/imgui-sfml/imgui-SFML.cpp ]]; then
        IMGUI_SFML_DIR=/mnt/c/Dev/imgui-sfml
    elif [[ -f C:/Dev/imgui-sfml/imgui-SFML.cpp ]]; then
        IMGUI_SFML_DIR=C:/Dev/imgui-sfml
    fi
fi

if [[ -z "$IMGUI_DIR" || ! -f "$IMGUI_DIR/imgui.cpp" ]]; then
    echo "ImGui not found. Set IMGUI_DIR to the folder containing imgui.cpp." >&2
    exit 1
fi

if [[ -z "$IMGUI_SFML_DIR" || ! -f "$IMGUI_SFML_DIR/imgui-SFML.cpp" ]]; then
    echo "ImGui-SFML not found. Set IMGUI_SFML_DIR to the folder containing imgui-SFML.cpp." >&2
    exit 1
fi

if command -v pkg-config >/dev/null 2>&1 && pkg-config --exists sfml-graphics sfml-window sfml-system sfml-audio; then
    read -r -a SFML_CFLAGS <<<"$(pkg-config --cflags sfml-graphics sfml-window sfml-system sfml-audio)"
    read -r -a SFML_LIBS <<<"$(pkg-config --libs sfml-graphics sfml-window sfml-system sfml-audio)"
else
    SFML_CFLAGS=()
    SFML_LIBS=(-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio)
fi

g++ -std=c++20 -Wall -Wextra -Wpedantic \
    src/*.cpp \
    external/glad/src/glad.c \
    "$IMGUI_DIR/imgui.cpp" \
    "$IMGUI_DIR/imgui_draw.cpp" \
    "$IMGUI_DIR/imgui_tables.cpp" \
    "$IMGUI_DIR/imgui_widgets.cpp" \
    "$IMGUI_SFML_DIR/imgui-SFML.cpp" \
    -Iexternal/glad/include \
    -Iinclude/orbital \
    -I"$IMGUI_DIR" \
    -I"$IMGUI_SFML_DIR" \
    "${SFML_CFLAGS[@]}" \
    "${SFML_LIBS[@]}" \
    -lGL \
    -o app

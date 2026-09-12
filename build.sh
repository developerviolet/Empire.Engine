#!/bin/bash
# Сборка X11Dialog с встроенной иконкой.
# 1. Встраивает PNG в объектный файл (objcopy)
# 2. Компилирует и линкует всё вместе
set -e
cd "$(dirname "$0")"

# Встраиваем иконку в объектник (символы _binary_*_png_start/end)
objcopy -I binary -O elf64-x86-64 -B i386:x86-64 \
    icons8-error-64.png icons8-error-64.o

# Компиляция и линковка
g++ X11Dialog.cpp icons8-error-64.o -o X11Dialog \
    $(pkg-config --cflags pangocairo) \
    -lxcb -lcairo \
    $(pkg-config --libs pangocairo)

echo "OK: ./X11Dialog"

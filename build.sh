#!/bin/sh

export TL_PATH=/c/me/Software/stm32/gnu-mcu-eclipse-arm-none-eabi-gcc-8.2.1-1.2-20190119-1237-win64/bin

# Don't use this due to a known bug: https://bugs.launchpad.net/gcc-arm-embedded/+bug/1810274
# export TL_PATH=/c/me/Software/stm32/gcc-arm-none-eabi-8-2018-q4-major-win32/bin

export PATH=$TL_PATH:$PATH

rm -rf build
cmake -DCMAKE_TOOLCHAIN_FILE=./cmake/arm.cmake -DCMAKE_BUILD_TYPE=Release -DPLATFORM=stm32 -S . -B build
cmake --build build

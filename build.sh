#!/bin/sh

# Don't use this due to a known bug: https://bugs.launchpad.net/gcc-arm-embedded/+bug/1810274
# export TL_PATH=/c/me/Software/stm32/gcc-arm-none-eabi-8-2018-q4-major-win32/bin

# exports moved to .bashrc
# export TL_PATH=/c/me/Software/stm32/gnu-mcu-eclipse-arm-none-eabi-gcc-8.2.1-1.2-20190119-1237-win64/bin
# export PATH=$TL_PATH:$PATH

export BUILD_TARGET='RELEASE'

# rm -rf build
# cmake -DCMAKE_TOOLCHAIN_FILE=./cmake/arm.cmake -DCMAKE_BUILD_TYPE=Release -DPLATFORM=stm32 -S . -B build
# cmake --build build

if [ $BUILD_TARGET == 'RELEASE' ]; then
  export TARGET_FOLDER=build/stm32/release
  mkdir -p $TARGET_FOLDER
  cd $TARGET_FOLDER
  cmake -DCMAKE_TOOLCHAIN_FILE=./cmake/arm.cmake -DCMAKE_BUILD_TYPE=Release -DPLATFORM=stm32 ../../..
fi

if [ $BUILD_TARGET == 'DEBUG' ]; then
  export TARGET_FOLDER=build/stm32/debug
  mkdir -p $TARGET_FOLDER
  cd $TARGET_FOLDER
  cmake -DCMAKE_TOOLCHAIN_FILE=./cmake/arm.cmake -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=stm32 ../../..
fi

cmake --build .

cd ../../..

if [ $BUILD_TARGET == 'RELEASE' ]; then
  make deploy
fi
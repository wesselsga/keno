#!/bin/bash

IN_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$IN_DIR/build"
GYP_HOME="$BUILD_DIR/gyp"

mkdir -p "$BUILD_DIR"
echo "IN_DIR=$IN_DIR"
echo "BUILD_DIR=$BUILD_DIR"

# download gyp
if [ ! -d "$GYP_HOME" ]; then
echo "Downloading gyp ..."
git clone "https://git.chromium.org/external/gyp.git" "$GYP_HOME"
fi

# what flavor of linux is this?
DISTRIB=$(cat /etc/*-release | grep '^ID=' | sed 's/ID=//')
echo "DISTRIB=$DISTRIB"

# set the processor target
PLATFORM="x86"
if [[ "$DISTRIB" == 'raspbian' ]]; then
PLATFORM="arm"
fi

# create dir structure for third party libs
mkdir -p "$BUILD_DIR/include"
mkdir -p "$BUILD_DIR/lib/$PLATFORM/release"
mkdir -p "$BUILD_DIR/lib/$PLATFORM/debug"

# download and build libuv
if [ ! -d "$BUILD_DIR/third_party/libuv" ]; then
echo "Downloading libuv ..."
git clone https://github.com/joyent/libuv.git "$BUILD_DIR/third_party/libuv"
git clone https://git.chromium.org/external/gyp.git "$BUILD_DIR/third_party/libuv/build/gyp"
cd "$BUILD_DIR/third_party/libuv"
./gyp_uv.py -f make
make -C out BUILDTYPE=Release
cp out/Release/libuv.a "$BUILD_DIR/lib/$PLATFORM/release"
mkdir -p "$BUILD_DIR/include/uv"
cp include/*.h "$BUILD_DIR/include/uv"
fi

# build free image
if [ ! -d "$BUILD_DIR/third_party/freeimage" ]; then
echo "Building freeimage ..."
unzip -q "$IN_DIR/third_party/freeimage.zip" -d "$BUILD_DIR/third_party/"
cd "$BUILD_DIR/third_party/freeimage"
make
cp Dist/FreeImage.h "$BUILD_DIR/include/freeimage.h"
mv Dist/libfreeimage.a "$BUILD_DIR/lib/$PLATFORM/release"
# make install DESTDIR=$BUILD_DIR
fi

# build free type
if [ ! -d "$BUILD_DIR/third_party/freetype-2.4.11" ]; then
echo "Building freetype ..."
unzip -q "$IN_DIR/third_party/freetype-2.4.11.zip" -d "$BUILD_DIR/third_party/"
cd "$BUILD_DIR/third_party/freetype-2.4.11"
make setup ansi
make
cp objs/libfreetype.a "$BUILD_DIR/lib/$PLATFORM/release"
cp -R include/* "$BUILD_DIR/include/"
fi

cd $IN_DIR

# download and build v8
V8_TARGET="ia32.release"
if [[ "$PLATFORM" == 'arm' ]]; then
V8_TARGET="arm.release"
fi

if [ ! -d "$BUILD_DIR/third_party/v8" ]; then
echo "Building v8 $V8_TARGET ..."
git clone https://github.com/v8/v8.git "$BUILD_DIR/third_party/v8"
unzip -q "$IN_DIR/third_party/icu.zip" -d "$BUILD_DIR/third_party/v8/third_party/"
git clone https://git.chromium.org/external/gyp.git "$BUILD_DIR/third_party/v8/build/gyp"
cd "$BUILD_DIR/third_party/v8"
make "$V8_TARGET"
cp out/$V8_TARGET/obj.target/tools/gyp/*.a "$BUILD_DIR/lib/$PLATFORM/release/"
cp out/$V8_TARGET/obj.target/third_party/icu/*.a "$BUILD_DIR/lib/$PLATFORM/release/"
mkdir "$BUILD_DIR/include/v8"
cp include/*.h "$BUILD_DIR/include/v8/"
fi

cd $IN_DIR

if [[ "$DISTRIB" == 'raspbian' ]]; then

   "$GYP_HOME/gyp" reno.gyp \
      --debug=all \
      --depth=. \
      -f make \
      --generator-output="$BUILD_DIR" \
      -Darch=$PLATFORM \
      -Ddistrib=raspi
else

   "$GYP_HOME/gyp" reno.gyp \
      --debug=all \
      --depth=. \
      -f make \
      --generator-output="$BUILD_DIR" \
      -Darch=$PLATFORM

fi


#echo "building v8..."
#tar -xzvf "$src_dir/v8.tar.gz" --directory="$build_dir/"
#cd "$build_dir/v8"
#make arm.release armv7=false vfp3=off hardfp=on

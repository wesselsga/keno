#!/bin/bash

# function to print to console with color
function log {
   echo "$(tput setaf 6)`date +%T` - $1$(tput sgr0)"
}

IN_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$IN_DIR/build"
GYP_HOME="$BUILD_DIR/gyp"

mkdir -p "$BUILD_DIR"
log "$(tput setaf 6)IN_DIR=$IN_DIR$(tput sgr0)"
log "BUILD_DIR=$BUILD_DIR"

# download gyp
if [ ! -d "$GYP_HOME" ]; then
log "Downloading gyp ..."
git clone "https://git.chromium.org/external/gyp.git" "$GYP_HOME"
fi

# what flavor of linux is this?
DISTRIB=$(cat /etc/*-release | grep '^ID=' | sed 's/ID=//')
if [ "$DISTRIB" == 'raspbian' ]; then
   DISTRIB='raspi';
fi
log "DISTRIB=$DISTRIB"

# set the processor target
PLATFORM="x86"
if [[ "$DISTRIB" == 'raspi' ]]; then
PLATFORM="arm"
fi

# create dir structure for third party libs
mkdir -p "$BUILD_DIR/include"
mkdir -p "$BUILD_DIR/lib/$PLATFORM/release"
mkdir -p "$BUILD_DIR/lib/$PLATFORM/debug"

# download and build libuv
if [ ! -d "$BUILD_DIR/third_party/libuv" ]; then
log "Downloading libuv ..."
git clone https://github.com/joyent/libuv.git "$BUILD_DIR/third_party/libuv"
git clone https://git.chromium.org/external/gyp.git "$BUILD_DIR/third_party/libuv/build/gyp"
cd "$BUILD_DIR/third_party/libuv"
./gyp_uv.py -f make
make -C out BUILDTYPE=Release
cp out/Release/libuv.a "$BUILD_DIR/lib/$PLATFORM/release"
mkdir -p "$BUILD_DIR/include/uv"
cp include/*.h "$BUILD_DIR/include/uv"
fi

# build freeimage
if [ ! -d "$BUILD_DIR/third_party/freeimage" ]; then
log "Building freeimage ..."
unzip -q "$IN_DIR/third_party/freeimage.zip" -d "$BUILD_DIR/third_party/"
cd "$BUILD_DIR/third_party/freeimage"
make
cp Dist/FreeImage.h "$BUILD_DIR/include/freeimage.h"
mv Dist/libfreeimage.a "$BUILD_DIR/lib/$PLATFORM/release"
# make install DESTDIR=$BUILD_DIR
fi

# build freetype
if [ ! -d "$BUILD_DIR/third_party/freetype-2.4.11" ]; then
log "Building freetype ..."
unzip -q "$IN_DIR/third_party/freetype-2.4.11.zip" -d "$BUILD_DIR/third_party/"
cd "$BUILD_DIR/third_party/freetype-2.4.11"
make setup ansi
make
cp objs/libfreetype.a "$BUILD_DIR/lib/$PLATFORM/release"
cp -R include/* "$BUILD_DIR/include/"
fi

cd $IN_DIR

# which v8 target are we building?
V8_TARGET=""
if [[ "$PLATFORM" == 'arm' ]]; then
   V8_TARGET="arm.release"
else
   V8_TARGET="ia32.release"
fi

# download v8 and its dependencies if we need to
if [ ! -d "$BUILD_DIR/third_party/v8" ]; then
   log "Downloading v8  ..."
   git clone https://github.com/v8/v8.git "$BUILD_DIR/third_party/v8"
   unzip -q "$IN_DIR/third_party/icu.zip" -d "$BUILD_DIR/third_party/v8/third_party/"
   git clone https://git.chromium.org/external/gyp.git "$BUILD_DIR/third_party/v8/build/gyp"
else
   V8_TARGET=""
fi

# on raspi; we need to force arm6
if [ "$DISTRIB" == 'raspi' ] && [ -n "$V8_TARGET" ]; then
   V8_TARGET="$V8_TARGET arm7=false vfp3=off hardfp=on" 
fi

# actually build v8 if we have a target options
if [[ -n "$V8_TARGET" ]]; then 
   log "Building v8 $V8_TARGET ..."
   cd "$BUILD_DIR/third_party/v8"
   make $V8_TARGET
   cp out/$V8_TARGET/obj.target/tools/gyp/*.a "$BUILD_DIR/lib/$PLATFORM/release/"
   cp out/$V8_TARGET/obj.target/third_party/icu/*.a "$BUILD_DIR/lib/$PLATFORM/release/"
   mkdir "$BUILD_DIR/include/v8"
   cp include/*.h "$BUILD_DIR/include/v8/"
fi

cd $IN_DIR

# finally; generate our projects
"$GYP_HOME/gyp" reno.gyp \
   --debug=all \
   --depth=. \
   -f make \
   --generator-output="$BUILD_DIR" \
   -Darch=$PLATFORM \
   -Ddistrib=$DISTRIB


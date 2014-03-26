#!/bin/bash

# function to print to console with color
function log {
   echo "$(tput setaf 6)`date +%T` - $1$(tput sgr0)"
}

IN_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$IN_DIR/build"
GYP_HOME="$BUILD_DIR/gyp"

# what flavor of linux is this?
DISTRIB=$(cat /etc/*-release | grep '^ID=' | sed 's/ID=//')
if [ "$DISTRIB" == 'raspbian' ]; then
   DISTRIB='raspi';
fi
log "DISTRIB=$DISTRIB"

mkdir -p "$BUILD_DIR"
log "IN_DIR=$IN_DIR"
log "BUILD_DIR=$BUILD_DIR"

# download gyp
if [ ! -d "$GYP_HOME" ]; then
   log "Downloading gyp ..."
   git clone "https://git.chromium.org/external/gyp.git" "$GYP_HOME"
fi

# set the processor (architecture)
ARCH="x86"
if [[ "$DISTRIB" == 'raspi' ]]; then
   ARCH="arm"
fi

# create dir structure for third party libs
mkdir -p "$BUILD_DIR/include"
mkdir -p "$BUILD_DIR/lib/$ARCH/release"
mkdir -p "$BUILD_DIR/lib/$ARCH/debug"

# download and build libuv
if [ ! -d "$BUILD_DIR/third_party/libuv" ]; then
   log "Downloading libuv ..."
   git clone https://github.com/joyent/libuv.git "$BUILD_DIR/third_party/libuv"
   git clone https://git.chromium.org/external/gyp.git "$BUILD_DIR/third_party/libuv/build/gyp"
   cd "$BUILD_DIR/third_party/libuv"
   log "Building libuv ..."
   ./gyp_uv.py -f make
   make -C out BUILDTYPE=Release
   
   log "Copying libuv lib/*.h ..." 
   cp out/Release/libuv.a "$BUILD_DIR/lib/$ARCH/release"
   mkdir -p "$BUILD_DIR/include/uv"
   cp include/*.h "$BUILD_DIR/include/uv"
fi

# build freeimage
if [ ! -d "$BUILD_DIR/third_party/freeimage" ]; then
   log "Building freeimage ..."
   unzip -q "$IN_DIR/third_party/freeimage.zip" -d "$BUILD_DIR/third_party/"
   cd "$BUILD_DIR/third_party/freeimage"
   make
   log "Copying freeimage lib/*.h ..."
   cp Dist/FreeImage.h "$BUILD_DIR/include/freeimage.h"
   mv Dist/libfreeimage.a "$BUILD_DIR/lib/$ARCH/release"
fi

# build freetype
if [ ! -d "$BUILD_DIR/third_party/freetype-2.4.11" ]; then
   log "Building freetype ..."
   unzip -q "$IN_DIR/third_party/freetype-2.4.11.zip" -d "$BUILD_DIR/third_party/"
   cd "$BUILD_DIR/third_party/freetype-2.4.11"
   make setup ansi
   make
   log "Copying freetype lib/*.h ..."
   cp objs/libfreetype.a "$BUILD_DIR/lib/$ARCH/release"
   cp -R include/* "$BUILD_DIR/include/"
fi

cd $IN_DIR

# which v8 target are we building?
V8_ARCH=$ARCH
V8_TARGET=

# v8 calls x86 'ia32'
if [[ $V8_ARCH == 'x86' ]]; then
   V8_ARCH="ia32"
fi

# download v8 and its dependencies if we need to
if [ ! -d "$BUILD_DIR/third_party/v8" ]; then
   log "Downloading v8 ..."
   git clone -b 3.24 https://github.com/v8/v8.git "$BUILD_DIR/third_party/v8"
   #unzip -q "$IN_DIR/third_party/icu.zip" -d "$BUILD_DIR/third_party/v8/third_party/"
   git clone https://git.chromium.org/external/gyp.git "$BUILD_DIR/third_party/v8/build/gyp"
   log "Patching cpu.cc in v8/src ..."
   cp "$IN_DIR/third_party/cpu.cc" "$BUILD_DIR/third_party/v8/src/"
else
   V8_ARCH=""
fi

# on raspi; we need to force arm6
V8_TARGET="$V8_ARCH.release snapshot=off i18nsupport=off"
if [ "$DISTRIB" == 'raspi' ] && [ -n "$V8_TARGET" ]; then
   #V8_TARGET="$V8_TARGET arm7=false vfp3=off hardfp=on" 
   #V8_TARGET="$V8_TARGET arm7=false hardfp=on vpfp3=off"
   V8_TARGET="$V8_TARGET arm_version=6 vfp3=off armfloatabi=hard"
fi

# actually build v8 if we have a target
if [[ -n $V8_ARCH ]]; then 
   log "Building v8: $V8_TARGET ..."
   cd "$BUILD_DIR/third_party/v8"
   
   make $V8_TARGET
   
   log "Copying v8 libs/*.h ..."
   mv out/$V8_ARCH.release/obj.target/tools/gyp/libv8_base.$V8_ARCH.a \
      "$BUILD_DIR/lib/$ARCH/release/libv8_base.a"
   mv out/$V8_ARCH.release/obj.target/tools/gyp/libv8_nosnapshot.$V8_ARCH.a \
      "$BUILD_DIR/lib/$ARCH/release/libv8_nosnapshot.a"
   #mv out/$V8_ARCH.release/obj.target/tools/gyp/libv8_snapshot.a \
   #   "$BUILD_DIR/lib/$ARCH/release/libv8_snapshot.a"
   mv out/$V8_ARCH.release/obj.target/third_party/icu/*.a "$BUILD_DIR/lib/$ARCH/release/"
   
   mkdir "$BUILD_DIR/include/v8"
   cp include/*.h "$BUILD_DIR/include/v8/"
fi

cd $IN_DIR

log "Generating make projects ..."

# finally; generate our projects
"$GYP_HOME/gyp" reno.gyp \
   --debug=all \
   --depth=. \
   -f make \
   --generator-output="$BUILD_DIR" \
   -Darch=$ARCH \
   -Ddistrib=$DISTRIB

log "Complete."



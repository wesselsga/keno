#!/bin/bash

IN_DIR="."
BUILD_DIR="$IN_DIR/build"
GYP_HOME="$BUILD_DIR/gyp"

# download gyp
if [ ! -d "%GYP_HOME" ]; then
echo "Downloading gyp..."
git clone "https://git.chromium.org/external/gyp.git" "$GYP_HOME"
fi

# what flavor of linux is this?
DISTRIB=$(cat /etc/*-release | grep '^ID=' | sed 's/ID=//')

echo $DISTRIB

if [[ "$DISTRIB" == 'raspbian' ]]; then

   "$GYP_HOME/gyp" reno.gyp \
      --debug=all \
      --depth=. \
      -f make \
      --generator-output="$BUILD_DIR" \
      -Darch=arm \
      -Ddistrib=raspi
else

   "$GYP_HOME/gyp" reno.gyp \
      --debug=all \
      --depth=. \
      -f make \
      --generator-output="$BUILD_DIR" \
      -Darch=x86

fi


#echo "building freeimage..."
#unzip "$src_dir/FreeImage3154.zip" -d "$build_dir/"
#cd "$build_dir/FreeImage"
#make
#make install DESTDIR=$build_dir

#echo "building zlib..."
#tar -xzvf "$src_dir/zlib-1.2.7.tar.gz" --directory="$build_dir/"
#cd "$build_dir/zlib-1.2.7"
#./configure
#make
#make install 

#echo "building libpng..."
#tar -xzvf "$src_dir/libpng-1.5.14.tar.gz" --directory="$build_dir/"
#cd "$build_dir/libpng-1.5.14"
#./configure
#make check
#make install

#echo "building v8..."
#tar -xzvf "$src_dir/v8.tar.gz" --directory="$build_dir/"
#cd "$build_dir/v8"
#make arm.release armv7=false vfp3=off hardfp=on

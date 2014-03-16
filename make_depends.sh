#!/bin/bash

in_dir="."
out_dir="$in_dir/build"
gyp_home="$in_dir/tools/gyp"

distrib=$(cat /etc/*-release | grep '^ID=' | sed 's/ID=//')

echo $distrib

if [[ "$distrib" == 'raspbian' ]]; then

   "$gyp_home/gyp" reno.gyp \
      --debug=all \
      --depth=. \
      -f make \
      --generator-output="$out_dir" \
      -Darch=arm \
      -Ddistrib=raspi
else

   "$gyp_home/gyp" reno.gyp \
      --debug=all \
      --depth=. \
      -f make \
      --generator-output="$out_dir" \
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

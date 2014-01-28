#!/bin/sh

cur_dir="."
src_dir="./deps"
build_dir="./build/deps"

rm -rf "$build_dir"
mkdir -p "$build_dir"

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

echo "building v8..."
tar -xzvf "$src_dir/v8.tar.gz" --directory="$build_dir/"
cd "$build_dir/v8"
make arm.release armv7=false vfp3=off hardfp=on

cd "$cur_dir"




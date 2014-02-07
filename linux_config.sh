#!/bin/bash

in_dir="."
out_dir="$in_dir/build"
gyp_home="$in_dir/tools/gyp"

"$gyp_home/gyp" reno.gyp \
--debug=all \
--depth=. \
-f make \
--generator-output="$out_dir" \
-Darch=x86



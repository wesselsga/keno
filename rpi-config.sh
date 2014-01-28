#!/bin/bash

in_dir="."
out_dir="$in_dir/build"
gyp_home="$in_dir/tools/gyp"

python "$gyp_home/gyp" firefly.gyp \
--debug=all \
--depth=. \
-f make \
--generator-output="$out_dir" \
-Darch=arm



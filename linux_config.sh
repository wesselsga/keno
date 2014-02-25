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


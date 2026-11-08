#!/usr/bin/env sh
set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
hashlink_dir=$(CDPATH= cd -- "$script_dir/../.." && pwd)
build_dir="$script_dir/build-linux"

export LD_LIBRARY_PATH="$hashlink_dir/linux_bin${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

cmake -S "$script_dir" -B "$build_dir" -DCMAKE_BUILD_TYPE=Release
cmake --build "$build_dir" --target recast.hdll --parallel

printf 'Built %s/recast.hdll\n' "$hashlink_dir"

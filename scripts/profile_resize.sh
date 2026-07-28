#!/usr/bin/env bash
set -euo pipefail

build_dir="${BUILD_DIR:-build-cuda}"
mkdir -p benchmark-results

if ! command -v nsys >/dev/null 2>&1; then
  echo "nsys not found; install Nsight Systems or run on the Jetson/GPU host." >&2
  exit 1
fi

nsys profile --trace=cuda,nvtx,osrt \
  --output=benchmark-results/resize-timeline \
  "./${build_dir}/resize_demo"


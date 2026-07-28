#!/usr/bin/env bash
set -euo pipefail

build_dir="${BUILD_DIR:-build-cpu}"
iterations="${ITERATIONS:-10}"
mkdir -p benchmark-results

if [[ ! -x "${build_dir}/resize_benchmark" ]]; then
  cmake -S . -B "${build_dir}" -DCMAKE_BUILD_TYPE=Release -DENABLE_CUDA=OFF
  cmake --build "${build_dir}" --parallel
fi

timestamp="$(date -u +%Y%m%dT%H%M%SZ)"
"./${build_dir}/resize_benchmark" "${iterations}" | \
  tee "benchmark-results/cpu-${timestamp}.csv"


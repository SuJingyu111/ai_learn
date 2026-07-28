#!/usr/bin/env bash
set -euo pipefail

mode="${1:-cpu}"
case "${mode}" in
  cpu)
    cuda_flag=OFF
    build_dir=build-cpu
    ;;
  cuda)
    cuda_flag=ON
    build_dir=build-cuda
    ;;
  *)
    echo "Usage: $0 [cpu|cuda]" >&2
    exit 2
    ;;
esac

cmake -S . -B "${build_dir}" -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_CUDA="${cuda_flag}" -DENABLE_STRICT_WARNINGS=ON
cmake --build "${build_dir}" --parallel
ctest --test-dir "${build_dir}" --output-on-failure
"./${build_dir}/resize_demo"


#!/usr/bin/env bash
set -euo pipefail

build_dir="${BUILD_DIR:-build-cpu}"
iterations="${ITERATIONS:-100}"
mkdir -p benchmark-results

# Only ever configure a CPU-only tree here. Reconfiguring an existing CUDA
# build directory with ENABLE_CUDA=OFF would silently discard its CUDA targets.
if [[ ! -x "${build_dir}/resize_benchmark" ]]; then
  if [[ -f "${build_dir}/CMakeCache.txt" ]] &&
     grep -q '^ENABLE_CUDA:BOOL=ON' "${build_dir}/CMakeCache.txt"; then
    echo "${build_dir} is a CUDA build tree; refusing to reconfigure it as CPU-only." >&2
    echo "Build it first with: ./scripts/build_and_test.sh cuda" >&2
    exit 2
  fi
  cmake -S . -B "${build_dir}" -DCMAKE_BUILD_TYPE=Release -DENABLE_CUDA=OFF
  cmake --build "${build_dir}" --parallel
fi

# Source identity for the evidence bundle. docs/mastery-gates.md makes
# "evidence 无法关联 commit" a hard blocker, so record the dirty marker
# explicitly instead of pretending the tree matched the commit.
CUDA_AI_COMMIT="${CUDA_AI_COMMIT:-$(git describe --always --dirty --tags 2>/dev/null || echo unversioned)}"
CUDA_AI_HOST="${CUDA_AI_HOST:-$(uname -srm)}"
export CUDA_AI_COMMIT CUDA_AI_HOST

timestamp="$(date -u +%Y%m%dT%H%M%SZ)"
"./${build_dir}/resize_benchmark" "${iterations}" | \
  tee "benchmark-results/cpu-${timestamp}.csv"

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
TT_COMMIT="${TT_COMMIT:-$(git describe --always --dirty --tags 2>/dev/null || echo unversioned)}"
TT_HOST="${TT_HOST:-$(uname -srm)}"
# The CPU model is the single most important fact about a CPU benchmark, and
# uname does not carry it. Platform-specific, so the script supplies it.
if [[ -z "${TT_CPU:-}" ]]; then
  if command -v sysctl >/dev/null 2>&1 && sysctl -n machdep.cpu.brand_string >/dev/null 2>&1; then
    TT_CPU="$(sysctl -n machdep.cpu.brand_string)"
  elif [[ -r /proc/cpuinfo ]]; then
    TT_CPU="$(awk -F': ' '/^model name/ {print $2; exit}' /proc/cpuinfo)"
  else
    TT_CPU="unrecorded"
  fi
fi
export TT_COMMIT TT_HOST TT_CPU

timestamp="$(date -u +%Y%m%dT%H%M%SZ)"
"./${build_dir}/resize_benchmark" "${iterations}" | \
  tee "benchmark-results/cpu-${timestamp}.csv"

#!/usr/bin/env bash
set -euo pipefail

if ! command -v clang-format >/dev/null 2>&1; then
  echo "clang-format not found; skipping local format check."
  exit 0
fi

files=()
while IFS= read -r file; do
  files+=("${file}")
done < <(find engine -type f \
  \( -name '*.cpp' -o -name '*.h' -o -name '*.cu' \) -print)

if ! clang-format --dry-run --Werror "${files[@]}"; then
  echo "Run: clang-format -i <changed C++/CUDA files>" >&2
  exit 1
fi
